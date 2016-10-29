/**
  * Copyright (c) <2016> granada <afernandez@cookinapps.io>
  *
  * This source code is licensed under the MIT license.
  *
  * Permission is hereby granted, free of charge, to any person obtaining a copy
  * of this software and associated documentation files (the "Software"), to deal
  * in the Software without restriction, including without limitation the rights
  * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  * copies of the Software, and to permit persons to whom the Software is
  * furnished to do so, subject to the following conditions:
  *
  * The above copyright notice and this permission notice shall be included in
  * all copies or substantial portions of the Software.
  *
  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  * SOFTWARE.
  *
  * Plugin Handler: Handles the lifecycle and communication of server side plugins.
  *
  */

#include "granada/plugin/plugin.h"

namespace granada{
  namespace plugin{

////
// static variables
//
    int PluginHandler::PLUGIN_BYTES_LIMIT_ = 0;
    int PluginHandler::SEND_MESSAGE_PLUGIN_GROUP_SIZE_ = 100;
    int PluginHandler::RUNNER_USE_FREQUENCY_LIMIT_ = 0;
    unsigned long long PluginHandler::uid_ = 0;
    std::mutex PluginHandler::uid_mtx_;
    granada::util::mutex::call_once PluginHandler::load_properties_call_once_;
    granada::util::mutex::call_once PluginHandler::functions_to_runner_call_once_;
//
////


    bool PluginHandler::Exists(){
      return cache()->Exists(plugin_handler_value_hash(),entity_keys::plugin_handler_id);
    }


    web::json::value PluginHandler::Init(const std::vector<std::string>& paths){

      web::json::value response = web::json::value::object();
      
      // copy paths of the plug-in repositories.
      paths_ = paths;

      // pre-load the plug-ins of the repositories with the
      // gien paths.
      int files_size = PreloadRepositories();
      if (files_size > PluginHandler::PLUGIN_BYTES_LIMIT_){

        // Byte limit exceeded, inform through the parameters of the
        // "ph-init-after" event that will be fired after.
		  response[utility::conversions::to_string_t(default_strings::plugin_error)] = web::json::value::string(utility::conversions::to_string_t(default_errors::plugin_bytes_limit_exceeded));
		  response[utility::conversions::to_string_t(default_strings::plugin_error_description)] = web::json::value::string(utility::conversions::to_string_t(default_error_descriptions::plugin_bytes_limit_exceeded));
      }

      // cache plug-in handler id, now plug-in handler exists.
      cache()->Write(plugin_handler_value_hash(),entity_keys::plugin_handler_id,id_);

      // store the repositories
      cache()->Write(plugin_handler_value_hash(),entity_keys::plugin_handler_repositories,granada::util::vector::stringify(paths_,","));

      // fire "ph-init-after" event
      Fire(default_strings::plugin_init_ph_event + "-" + default_strings::plugin_after, response, [&response](const web::json::value& data){
        const web::json::value& new_parameters = granada::util::json::first(granada::util::json::as_object(data,entity_keys::plugin_parameter_data));
        if (!new_parameters.is_null()){
          response = granada::util::json::as_object(new_parameters,entity_keys::plugin_parameter_data);
        }
      }, [](const web::json::value& data){});

      return response;
    }


    void PluginHandler::Stop(){

      // remove all cached plugins linked with this plugin handler id.
      
      // remove plug-in stores.
      cache()->Destroy(plugin_store_hash(id_,"*"));

      // remove plug-in loader value.
      cache()->Destroy(plugin_loader_value_hash("*"));

      // remove event loaders from the cache.
      cache()->Destroy(plugin_event_value_hash("*"));

      // remove plugin values stored in the cache.
      cache()->Destroy(plugin_value_hash("*"));

      // remove plug-in handler values from the cache.
      cache()->Destroy(plugin_handler_value_hash());
    }


    void PluginHandler::Reset(){

      // retrieve the repositories
      const std::string& paths_str = cache()->Read(plugin_handler_value_hash(),entity_keys::plugin_handler_repositories);
      std::vector<std::string> paths;
      granada::util::string::split(paths_str,',',paths);

      // stop plug-in handler
      Stop();

      // initialize plug-in handler with the paths of
      // the plug-ins repositories.
      Init(paths);
    }


    bool PluginHandler::AddPluginLoader(web::json::value& header, const std::string& configuration, const std::string& script){

      const bool& malformed_parameters = configuration.empty() || script.empty() || header.is_null() || !header.is_object();
      
      if (malformed_parameters){
        return false;
      }else{
        std::string plugin_id = granada::util::json::as_string(header,entity_keys::plugin_header_id);

        if (plugin_id.empty()){
          // the plug-in does not have an id, so assign
          // an unique number as id.
          plugin_id = GetUID();
		  header[utility::conversions::to_string_t(entity_keys::plugin_header_id)] = web::json::value::string(utility::conversions::to_string_t(plugin_id));
        }

        // store plug-in loader values in the cache.
        {
          const std::string& plugin_loader_hash = plugin_loader_value_hash(plugin_id);
          cache()->Write(plugin_loader_hash,entity_keys::plugin_header_id,plugin_id);
		  cache()->Write(plugin_loader_hash, entity_keys::plugin_header, utility::conversions::to_utf8string(header.serialize()));
          cache()->Write(plugin_loader_hash,entity_keys::plugin_configuration,configuration);
          cache()->Write(plugin_loader_hash,entity_keys::plugin_script,script);
        }

        // add event loaders so the plug-in
        // is loaded when the first time one of
        // the events is fired.
        AddLoadEvent(plugin_id,header);

        // plug-in successfully preloaded, it is ready to be loaded and added.
        return true;
      }
    }


    void PluginHandler::RemovePluginLoader(const std::string& plugin_id){
      cache()->Destroy(plugin_loader_value_hash(plugin_id));
    }


    void PluginHandler::AddLoadEvent(const std::string& event_name,const std::string& plugin_id,const web::json::value& plugin_loader){

      const bool& malformed_parameters = event_name.empty() || plugin_id.empty() || plugin_loader.is_null() || !plugin_loader.is_object();

      if (!malformed_parameters){
        
        // retrieve the cached event loaders and parse them to a JSON object.
        const std::string& event_hash = plugin_event_value_hash(event_name);
        web::json::value cached_event_loaders = granada::util::string::to_json(cache()->Read(event_hash,entity_keys::plugin_event_loader));

        // add the plug-in event loader to the loaders.
		cached_event_loaders[utility::conversions::to_string_t(plugin_id)] = plugin_loader;

        // store the loaders again in the cache.
		cache()->Write(event_hash, entity_keys::plugin_event_loader, utility::conversions::to_utf8string(cached_event_loaders.serialize()));
      }
    }


    void PluginHandler::AddLoadEvent(const std::string& plugin_id,const web::json::value& header){
      
      const bool& malformed_parameters = plugin_id.empty() || header.is_null() || !header.is_object();

      if (!malformed_parameters){

        // retrieve the loader JSON object from the header JSON.
        const web::json::value& loader = granada::util::json::as_object(header,entity_keys::plugin_header_loader);
        bool load_and_add = false;
        bool lazy_load = false;
        
        if (loader.size()>0){

          // loader has content.
          
          // check if eager load is explicited in the loader JSON object,
          // if so, load and add the plug-in now.
          const std::string& load = granada::util::json::as_string(loader,entity_keys::plugin_loader_load);
          
          if (load!=default_strings::plugin_eager_load){

            // lazy load plug-in.

            // check if there are events specified on which the plug-in
            // has to be loaded, if there are not, load plug-in the
            // first time it is requested.
            const web::json::value& events = granada::util::json::as_array(loader,entity_keys::plugin_loader_events);
            if (events.size()>0){

              // load plug-in the first time one of the explicited events is fired.

              bool one_load_event_added = false;
              
              // check if the events are of type string.              
              for (auto it = events.as_array().cbegin(); it != events.as_array().cend(); ++it){
                if (it->is_string()){

                  // add loader event so when the event is fired
                  // the plug-in is loaded.
				  AddLoadEvent(utility::conversions::to_utf8string(it->as_string()), plugin_id, loader);
                  one_load_event_added = true;
                }
              }

              if (!one_load_event_added){

                // data is malformed or not understandable in
                // this function, events where expected to be strings, maybe
                // you want to override the function to accept other
                // type of data or correct the plug-in loader,
                // otherwise plug-in will be loaded the first time an
                // event that the plug-in is listening is fired.
                lazy_load = true;
              }

            }else{
              
              // loader has no events indicated.
              // Lazy load plug-in the first time
              // an event that the plug-in is listening is fired.
              lazy_load = true;
            }
          }else{

            // eager load case:
            // loader indicates this plug-in should be loaded now!
            load_and_add = true;
          }
        }else{

          // there is no loader, or loader is empty so the plug-in
          // will be loaded the first time an event that the plug-in
          // is listening is fired.
          lazy_load = true;
        }


        if (lazy_load){

          // by default lazy load plug-in the first time
          // an event it is listening is fired.
          // look at the events, and add a loader for each event.
          const web::json::value& events = granada::util::json::as_array(header,entity_keys::plugin_header_events);

          if (events.size()>0){

            // load plug-in the first time one of the events is fired.

            bool one_load_event_added = false;

            // check if the events are of type string.
            for (auto it = events.as_array().cbegin(); it != events.as_array().cend(); ++it){
              if (it->is_string()){

                // add loader event so when the event is fired
                // the plug-in is loaded.
				AddLoadEvent(utility::conversions::to_utf8string(it->as_string()), plugin_id, loader);
                one_load_event_added = true;
              }
            }

            if (!one_load_event_added){

                // data is malformed or not understandable in
                // this function, events where expected to be strings, maybe
                // you want to override the function to accept other
                // type of data or correct the plug-in loader,
                // otherwise plug-in will be loaded after plug-in handler
                // initialization.
                AddLoadEvent(default_strings::plugin_init_ph_event + "-" + default_strings::plugin_after,plugin_id,loader);
            }

          }else{

            // as there are no events in the header
            // load the plug-in after plug-in handler initialization.
            AddLoadEvent(default_strings::plugin_init_ph_event + "-" + default_strings::plugin_after,plugin_id,loader);
          }
        }else{

          if (load_and_add){

            // eager load. Load and add the plug-in now.
            const std::unique_ptr<granada::plugin::Plugin>& plugin = plugin_factory()->Plugin_unique_ptr(this,plugin_id);
            if (!plugin->Exists() && Load(plugin.get(),loader)){

              Add(plugin.get());

              // remove plug-in loader to prevent plug-in from being added again.
              RemovePluginLoader(plugin_id);
            }
          }
        }
      }
    }


    void PluginHandler::RemoveLoadEvent(const std::string& event_name){
      cache()->Destroy(plugin_event_value_hash(event_name),entity_keys::plugin_event_loader);
    }


    bool PluginHandler::Add(const std::string& header, const std::string& configuration, const std::string& script){

      const bool& malformed_parameters = script.empty() || header.empty() || configuration.empty();

      if (malformed_parameters){
        return false;
      }else{

        // parse header and configuration into JSON.
        web::json::value header_json = granada::util::string::to_json(header);
        const web::json::value& configuration_json = granada::util::string::to_json(configuration);

        // add plug-in, now plug-in can be run.
        return Add(header_json,configuration_json,script);
      }
    }


    bool PluginHandler::Add(web::json::value& header, const web::json::value& configuration, const std::string& script){
      
      const bool& malformed_parameters = script.empty() || !header.is_object() || !configuration.is_object();

      if (malformed_parameters){
        return false;
      }else{

        // instanciate a plug-in with the header, the configuration and the script, and
        // add it so it can be run.
        return Add(plugin_factory()->Plugin_unique_ptr(this,header,configuration,script).get());
      }      
    }


    bool PluginHandler::Add(granada::plugin::Plugin* plugin){

      // create empty parameters.
      web::json::value parameters = web::json::value::object();
      
      // adds the plug-in with empty parameters, now plug-in can be run.
      return Add(plugin,parameters);
    }


    bool PluginHandler::Add(granada::plugin::Plugin* plugin, web::json::value& parameters){

      if (plugin->Exists()){

        // plug-in is already added.
        return true;
      }else{

        // only add plug-in if it is not already added.

        std::string plugin_id = plugin->GetId();
        web::json::value header;

        bool id_assigned = false;

        if (plugin_id.empty()){

          // as plug-in does not have an id
          // assign one based in a sequence
          plugin_id = GetUID();
          header = plugin->GetHeader();
		  header[utility::conversions::to_string_t(entity_keys::plugin_header_id)] = web::json::value::string(utility::conversions::to_string_t(plugin_id));

          id_assigned = true;
        }

        // check if plug-in extends another
        // plugin that has aleady been loaded.
        // If so extend it.
        Extend(plugin);

        bool extended = false;
        
        if (!id_assigned){

          // check if plug-in is extended by OTHERS.
          // only plug-ins having an id can be extended,
          // if the id of the plug-in has just been assigned,
          // there is no way other plug-in can know it thus
          // extend it.
          extended = ApplyExtensions(plugin);
        }

        web::json::value event_parameters = web::json::value::object();
		event_parameters[utility::conversions::to_string_t(entity_keys::plugin_parameter_id)] = web::json::value::string(utility::conversions::to_string_t(plugin_id));
		event_parameters[utility::conversions::to_string_t(entity_keys::plugin_parameter_data)] = parameters;

        const std::string& plugin_add_after_event = default_strings::plugin_add_event + "-" + default_strings::plugin_after;

        if (extended){

          // do not manipulate an event that has been extended,
          // it has became an inactive plugin, its extensions
          // will do the job.
          // fire plug-in add after event.
          Fire(plugin_add_after_event,event_parameters);
          Fire(plugin_id + "-" + plugin_add_after_event,event_parameters);
        }else{

          // plug-in has not been extended, it is active and it can be run.

          bool run_plugin = false;

          if (header.is_null()){
            header = plugin->GetHeader();
          }
          
		  if (header.has_field(utility::conversions::to_string_t(entity_keys::plugin_header_events))){

			const web::json::value& events = header.at(utility::conversions::to_string_t(entity_keys::plugin_header_events));
            if (events.is_array() && events.size() > 0){

              // retrieve the way we are going to load the plugin for
              // each event and store it in the cache.

              for(auto it = events.as_array().cbegin(); it != events.as_array().cend(); ++it){
                if (it->is_string()){

                  // add the plug-in to event plug-in list so the plug-in is executed
                  // when event is fired.
				  AddEventListener(utility::conversions::to_utf8string(it->as_string()), plugin_id);

                }
              }
            }else{
              if (plugin->IsRunnable()){
                run_plugin = true;
              }
            }
          }else{
            if (plugin->IsRunnable()){
              run_plugin = true;
            }
          }

          // store plug-in values in the cache.
          {
            const std::string& plugin_hash = plugin_value_hash(plugin_id);
            cache()->Write(plugin_hash,entity_keys::plugin_header_id,plugin_id);
            cache()->Write(plugin_hash,entity_keys::plugin_script,plugin->GetScript());
			cache()->Write(plugin_hash, entity_keys::plugin_header, utility::conversions::to_utf8string(header.serialize()));
			cache()->Write(plugin_hash, entity_keys::plugin_configuration, utility::conversions::to_utf8string(plugin->GetConfiguration().serialize()));
          }

          // fire plug-in add after event.
          Fire(plugin_add_after_event,event_parameters);
          Fire(plugin_id + "-" + plugin_add_after_event,event_parameters);

          if (run_plugin){
            // if no events specified, run plug-in when added.
            Run(plugin,parameters,"",[](const web::json::value& data){},[](const web::json::value& data){});
          }
        }
		return true;
      }
    }


    void PluginHandler::Extend(granada::plugin::Plugin* plugin){
      if (plugin!=nullptr){
        const web::json::value& extends = granada::util::json::as_array(plugin->GetHeader(),entity_keys::plugin_header_extends);
        Extend(extends.as_array(),plugin);
      }

    }


    void PluginHandler::Extend(granada::plugin::Plugin* extended_plugin, granada::plugin::Plugin* plugin){
      if (extended_plugin!=nullptr && plugin!=nullptr){
        web::json::value extends = web::json::value::array(1);
		extends[0] = web::json::value::string(utility::conversions::to_string_t(extended_plugin->GetId()));
        Extend(extends.as_array(),plugin);
      }
    }


    void PluginHandler::AddExtension(const std::string& extended_plugin_id,const std::string& plugin_id){

      const bool& malformed_parameters = extended_plugin_id.empty() || plugin_id.empty();

      if (!malformed_parameters){

        const std::string& plugin_value_hash_str = plugin_value_hash(extended_plugin_id);
        
        // retrieve the cached extensions of the given plug-in.
        std::string cached_plugin_extended_str = cache()->Read(plugin_value_hash_str,entity_keys::plugin_extension_ids);
       
        if (cached_plugin_extended_str.empty()){
          // add extension to the plug-in.
          cached_plugin_extended_str = plugin_id;
        }else{

          // only add plug-in id if it has not already been added, to prevent the plug-in
          // to be extended twice.
          std::vector<std::string> plugin_ids;
          granada::util::string::split(cached_plugin_extended_str,',',plugin_ids);
          if (std::find(plugin_ids.begin(), plugin_ids.end(), plugin_id) == plugin_ids.end()){
            cached_plugin_extended_str += "," + plugin_id;
          }
        }

        // store the plug-in extensions again in the cache.
        cache()->Write(plugin_value_hash_str,entity_keys::plugin_extension_ids,cached_plugin_extended_str);
      }
    }


    bool PluginHandler::ApplyExtensions(granada::plugin::Plugin* plugin){
      
      bool is_extended = false;

      // retrieve the plug-ins that extend this plug-in and extend them.
      std::string plugin_ids_str = cache()->Read(plugin_value_hash(plugin->GetId()),entity_keys::plugin_extension_ids);
      
      if (!plugin_ids_str.empty()){
        
        std::vector<std::string> plugin_ids;
        granada::util::string::split(plugin_ids_str,',',plugin_ids);
        for (auto it = plugin_ids.begin(); it != plugin_ids.end(); ++it){
          const std::unique_ptr<granada::plugin::Plugin>& extension_plugin = GetPluginById(*it);

          // check if extension plug-in has been added.
          if (extension_plugin.get()!=nullptr){

            // extend plug-in
            Extend(plugin,extension_plugin.get());
            is_extended = true;
          }
        }

        RemoveExtensions(plugin->GetId());
      }

      return is_extended;
    }


    void PluginHandler::RemoveExtension(const std::string& extended_plugin_id, const std::string& plugin_id){

      const bool& malformed_parameters = extended_plugin_id.empty() || plugin_id.empty();

      if (!malformed_parameters){

        const std::string& plugin_value_hash_str = plugin_value_hash(extended_plugin_id);
        const std::string& plugin_ids_str = cache()->Read(plugin_value_hash_str,entity_keys::plugin_extension_ids);
        
        if (!plugin_ids_str.empty()){

          std::vector<std::string> plugin_ids;
          granada::util::string::split(plugin_ids_str,',',plugin_ids);

          // create a new plug-in list that exclude the plug-in.
          std::string new_plugin_ids_str = "";
          int i = 0;
          for (auto it = plugin_ids.begin(); it != plugin_ids.end(); ++it){
            if (*it != plugin_id){
              if (i==0){
                i=1;
              }else{
                new_plugin_ids_str += ",";
              }
              new_plugin_ids_str += *it;
            }
          }

          // rewrite extension plug-ins list
          cache()->Write(plugin_value_hash_str,entity_keys::plugin_extension_ids,new_plugin_ids_str);

        }
      }
    }


    void PluginHandler::RemoveExtensions(const std::string& plugin_id){
      cache()->Destroy(plugin_value_hash(plugin_id),entity_keys::plugin_extension_ids);
      cache()->Destroy(plugin_value_hash(plugin_id),entity_keys::plugin_extended);
    }


    void PluginHandler::AddEventListener(const std::string& event_name, const std::string& plugin_id){

      const bool& malformed_parameters = event_name.empty() || plugin_id.empty();

      if (!malformed_parameters){

        // retrieve the plug-ins list to fire when the event is fired
        const std::string& event_value_hash = plugin_event_value_hash(event_name);
        std::string event_plugins_ids = cache()->Read(event_value_hash,entity_keys::plugin_event_ids);
        
        // add plug-in to the event plug-ins list.
        if (event_plugins_ids.empty()){
          event_plugins_ids += plugin_id;
        }else{

          // only add plug-in id if it has not already been added, to prevent the plug-in
          // to run twice on event firing.
          std::vector<std::string> plugin_ids;
          granada::util::string::split(event_plugins_ids,',',plugin_ids);
          if (std::find(plugin_ids.begin(), plugin_ids.end(), plugin_id) == plugin_ids.end()){
            event_plugins_ids += "," + plugin_id;
          }
        }

        // store the new event plug-in ids list.
        cache()->Write(event_value_hash,entity_keys::plugin_event_ids,event_plugins_ids);

        // clean cached script.
        // contains all the plug-ins listening to
        // one event in one script.
        cache()->Destroy(event_value_hash,entity_keys::plugin_event_script);
      }
    }


    void PluginHandler::RemoveEventListener(const std::string& event_name, const std::string& plugin_id){

      const bool& malformed_parameters = event_name.empty() || plugin_id.empty();

      if (!malformed_parameters){
        const std::string& event_value_hash = plugin_event_value_hash(event_name);
        const std::string& event_plugins_ids = cache()->Read(event_value_hash,entity_keys::plugin_event_ids);

        if (!event_plugins_ids.empty()){

          std::vector<std::string> plugin_ids;
          granada::util::string::split(event_plugins_ids,',',plugin_ids);

          // create a new plug-in list that exclude the plug-in
          std::string new_event_plugins_ids = "";
          int i = 0;
          for (auto it = plugin_ids.begin(); it != plugin_ids.end(); ++it){
            if (*it != plugin_id){
              if (i==0){
                i=1;
              }else{
                new_event_plugins_ids += ",";
              }
              new_event_plugins_ids += *it;
            }
          }

          // rewrite event plug-in list
          cache()->Write(event_value_hash,entity_keys::plugin_event_ids,new_event_plugins_ids);

          // clean cached scripts
          cache()->Destroy(event_value_hash,entity_keys::plugin_event_script);
        }
      }
    }


    void PluginHandler::RemoveEventListeners(const std::string& plugin_id){
      if (!plugin_id.empty()){
        RemoveEventListeners(GetPluginById(plugin_id).get());
      }
    }


    void PluginHandler::RemoveEventListeners(granada::plugin::Plugin* plugin){
      if (plugin!=nullptr){
        const web::json::value& header = plugin->GetHeader();
        const web::json::value& events = granada::util::json::as_array(header,entity_keys::plugin_header_events);
        for (auto it = events.as_array().cbegin(); it != events.as_array().cend(); ++it){
          if (it->is_string()){
			  RemoveEventListener(utility::conversions::to_utf8string(it->as_string()), plugin->GetId());
          }
        }
      }
    }


    web::json::value PluginHandler::RemoveEventListeners(const web::json::value& parameters){

      web::json::value response = web::json::value::object();
      std::string error;
      std::string error_description;

      // check if all needed parameters are present.
      if (!parameters.is_null()
		  && parameters.has_field(utility::conversions::to_string_t(entity_keys::plugin_parameter_plugin_handler_id))
		  && parameters.has_field(utility::conversions::to_string_t(entity_keys::plugin_parameter_plugin_id))){

		const web::json::value& plugin_handler_id = parameters.at(utility::conversions::to_string_t(entity_keys::plugin_parameter_plugin_handler_id));
		const web::json::value& plugin_id = parameters.at(utility::conversions::to_string_t(entity_keys::plugin_parameter_plugin_id));

        // check if the needed parameters are of the correct type
        if (plugin_handler_id.is_string()
            && plugin_id.is_string()){

			const std::string& plugin_handler_id_str = utility::conversions::to_utf8string(plugin_handler_id.as_string());
			const std::string& plugin_id_str = utility::conversions::to_utf8string(plugin_id.as_string());

          if (plugin_handler_id_str.empty() || plugin_id_str.empty()){

            // Inform of error: malformed parameters.
            error = default_errors::plugin_malformed_parameters;
            error_description = default_error_descriptions::plugin_malformed_parameters;
          }else{
            const std::unique_ptr<granada::plugin::PluginHandler>& plugin_handler = plugin_factory()->PluginHandler_unique_ptr(plugin_handler_id_str);
            plugin_handler->RemoveEventListeners(plugin_id_str);
          }
        }else{
          
          // Inform of error: malformed parameters.
          error = default_errors::plugin_malformed_parameters;
          error_description = default_error_descriptions::plugin_malformed_parameters;
        }

      }else{

        // Inform of error: missing_parameter
        error = default_errors::plugin_missing_parameter;
        error_description = default_error_descriptions::plugin_missing_parameter;
      }

      if (!error.empty()){

        // respond with a JSON with an error code and an error_description like:
        // { "error" : "missing_parameter", "error_description" : "One or more of the given parameters is missing."}
		  response[utility::conversions::to_string_t(default_strings::plugin_error)] = web::json::value::string(utility::conversions::to_string_t(error));
		  response[utility::conversions::to_string_t(default_strings::plugin_error_description)] = web::json::value::string(utility::conversions::to_string_t(error_description));
      }

      return response;
    }


    std::unique_ptr<granada::plugin::Plugin> PluginHandler::GetPluginById(const std::string& plugin_id){

      const bool& malformed_parameters = plugin_id.empty() || id_.empty();

      if (!malformed_parameters){

        // retrieve plug-in values: header,configuration and script.
        const std::string& plugin_value_hash_str = plugin_value_hash(plugin_id);
        const std::string& header_str = cache()->Read(plugin_value_hash_str,entity_keys::plugin_header);
        const std::string& configuration_str = cache()->Read(plugin_value_hash_str,entity_keys::plugin_configuration);
        const std::string& script = cache()->Read(plugin_value_hash_str,entity_keys::plugin_script);

        const bool malformed_plugin = script.empty() || header_str.empty() || configuration_str.empty();

        if (!malformed_plugin){

          // parse the plug-in header and configuration into JSON objects.
          const web::json::value& header = granada::util::string::to_json(header_str);
          const web::json::value& configuration = granada::util::string::to_json(configuration_str);

          // instanciate a plug-in and return its pointer
          return plugin_factory()->Plugin_unique_ptr(this,header,configuration,script);
        }
      }

      // plug-in does not exist with the provided id
      // or its values are malformed,
      // return null pointer plugin. 
      return std::unique_ptr<granada::plugin::Plugin>(nullptr);
    }




    void PluginHandler::Run(const std::string& plugin_id, web::json::value& parameters, const std::string& event_name, function_void_json success, function_void_json failure){
      
      // retrieve added plug-in with given id.
      const std::unique_ptr<granada::plugin::Plugin>& plugin = GetPluginById(plugin_id);
      
      if (plugin.get() == nullptr){

        // plug-in with given id not found, call failure callback
        // with undefined plug-in error and error description.
        web::json::value response = web::json::value::object();
		response[utility::conversions::to_string_t(default_strings::plugin_error)] = web::json::value::string(utility::conversions::to_string_t(default_errors::plugin_undefined_plugin));
		response[utility::conversions::to_string_t(default_strings::plugin_error_description)] = web::json::value::string(utility::conversions::to_string_t(default_error_descriptions::plugin_undefined_plugin));
        failure(response);
      }else{

        // plug-in exists, run it.
        Run(plugin.get(),parameters,event_name,success,failure);
      }
    }


    void PluginHandler::Run(granada::plugin::Plugin* plugin, web::json::value& parameters,const std::string& event_name, function_void_json success, function_void_json failure){
      web::json::value response = web::json::value::object();
	  response[utility::conversions::to_string_t(default_strings::plugin_error)] = web::json::value::string(utility::conversions::to_string_t(default_errors::plugin_server_error));
	  response[utility::conversions::to_string_t(default_strings::plugin_error_description)] = web::json::value::string(utility::conversions::to_string_t(default_error_descriptions::plugin_server_error));
      failure(response);
    }


    web::json::value PluginHandler::Run(const web::json::value& parameters){

      web::json::value response;
      std::string error;
      std::string error_description;

      // check if all needed parameters are present.
      if (!parameters.is_null()
		  && parameters.has_field(utility::conversions::to_string_t(entity_keys::plugin_parameter_id))
		  && parameters.has_field(utility::conversions::to_string_t(entity_keys::plugin_parameters))
		  && parameters.has_field(utility::conversions::to_string_t(entity_keys::plugin_parameter_plugin_handler_id))
		  && parameters.has_field(utility::conversions::to_string_t(entity_keys::plugin_parameter_plugin_id))){

		  const web::json::value& id = parameters.at(utility::conversions::to_string_t(entity_keys::plugin_parameter_id));
		  web::json::value run_parameters = parameters.at(utility::conversions::to_string_t(entity_keys::plugin_parameters));
		  const web::json::value& plugin_handler_id = parameters.at(utility::conversions::to_string_t(entity_keys::plugin_parameter_plugin_handler_id));
		  const web::json::value& plugin_id = parameters.at(utility::conversions::to_string_t(entity_keys::plugin_parameter_plugin_id));

        // check if the needed parameters are of the correct type
        if (id.is_string()
            && run_parameters.is_object()
            && plugin_handler_id.is_string()
            && plugin_id.is_string()){
			
			const std::string& id_str = utility::conversions::to_utf8string(id.as_string());
			const std::string& plugin_handler_id_str = utility::conversions::to_utf8string(plugin_handler_id.as_string());
			const std::string& plugin_id_str = utility::conversions::to_utf8string(plugin_id.as_string());

          if (id_str.empty() || plugin_id_str.empty() || plugin_handler_id_str.empty()){

            // Inform of error: malformed parameters.
            error = default_errors::plugin_malformed_parameters;
            error_description = default_error_descriptions::plugin_malformed_parameters;
          }else{

            // retrieve the plug-in using a plug-in handler.
            std::unique_ptr<granada::plugin::Plugin> plugin;
            {
              const std::unique_ptr<granada::plugin::PluginHandler>& plugin_handler = plugin_factory()->PluginHandler_unique_ptr(plugin_handler_id_str);
              plugin = plugin_handler->GetPluginById(plugin_id_str);
            }

            // check if plug-in with given id exists.
            if (plugin.get() == nullptr){

              // it does not exists, return an
              // undefined plug-in error.
              response = web::json::value::object();
			  response[utility::conversions::to_string_t(default_strings::plugin_error)] = web::json::value::string(utility::conversions::to_string_t(default_errors::plugin_undefined_plugin));
			  response[utility::conversions::to_string_t(default_strings::plugin_error_description)] = web::json::value::string(utility::conversions::to_string_t(default_error_descriptions::plugin_undefined_plugin));
            }else{

              // plug-in with given id exists,
              // run plug-in
              Run(plugin.get(),run_parameters,plugin_id_str,[&plugin_id_str,&response](const web::json::value& data){
                
                web::json::value response_data = web::json::value::object();
				response_data[utility::conversions::to_string_t(plugin_id_str)] = std::move(data);
                response = web::json::value::object();
				response[utility::conversions::to_string_t(entity_keys::plugin_parameter_data)] = std::move(response_data);

              },[&response](const web::json::value& data){

                // failure running plug-in
                response = std::move(data);
              });
            }
          }
        }else{
          
          // Inform of error: malformed parameters.
          error = default_errors::plugin_malformed_parameters;
          error_description = default_error_descriptions::plugin_malformed_parameters;
        }

      }else{

        // Inform of error: missing_parameter
        error = default_errors::plugin_missing_parameter;
        error_description = default_error_descriptions::plugin_missing_parameter;
      }

      if (!error.empty()){

        // respond with a JSON with an error code and an error_description like:
        // { "error" : "missing_parameter", "error_description" : "One or more of the given parameters is missing."}
        response = web::json::value::object();
		response[utility::conversions::to_string_t(default_strings::plugin_error)] = web::json::value::string(utility::conversions::to_string_t(error));
		response[utility::conversions::to_string_t(default_strings::plugin_error_description)] = web::json::value::string(utility::conversions::to_string_t(error_description));
      }

      return response;
    }


    void PluginHandler::Remove(const std::string& plugin_id){

      if (!plugin_id.empty()){

        web::json::value parameters = web::json::value::object();
		parameters[utility::conversions::to_string_t(entity_keys::plugin_id)] = web::json::value::string(utility::conversions::to_string_t(plugin_id));

        const std::string& plugin_remove_before_event = default_strings::plugin_remove_event + "-" + default_strings::plugin_before;
        Fire(plugin_remove_before_event,parameters);
        Fire(plugin_id + "-" + plugin_remove_before_event,parameters);
        
        // remove no possibility of loading again.
        RemovePluginLoader(plugin_id);

        // no more listen to events.
        RemoveEventListeners(plugin_id);

        // remove extensions.
        RemoveExtensions(plugin_id);

        // remove plug-in values
        cache()->Destroy(plugin_store_hash(id_,plugin_id));

        // remove plug-in values from the cache.
        cache()->Destroy(plugin_value_hash(plugin_id));

        const std::string& plugin_remove_after_event = default_strings::plugin_remove_event + "-" + default_strings::plugin_after;
        Fire(plugin_remove_after_event,parameters);
        Fire(plugin_id + "-" + plugin_remove_after_event,parameters);
      }

    }


    web::json::value PluginHandler::Remove(const web::json::value& parameters){

      web::json::value response = web::json::value::object();
      std::string error;
      std::string error_description;

      // check if all needed parameters are present.
      if (!parameters.is_null()
		  && parameters.has_field(utility::conversions::to_string_t(entity_keys::plugin_parameter_id))
		  && parameters.has_field(utility::conversions::to_string_t(entity_keys::plugin_parameter_plugin_handler_id))
		  && parameters.has_field(utility::conversions::to_string_t(entity_keys::plugin_parameter_plugin_id))){

		const web::json::value& id = parameters.at(utility::conversions::to_string_t(entity_keys::plugin_parameter_id));
		const web::json::value& plugin_handler_id = parameters.at(utility::conversions::to_string_t(entity_keys::plugin_parameter_plugin_handler_id));
		const web::json::value& plugin_id = parameters.at(utility::conversions::to_string_t(entity_keys::plugin_parameter_plugin_id));

        // check if the needed parameters are of the correct type
        if (id.is_string() && plugin_handler_id.is_string() && plugin_id.is_string()){
			
		  const std::string& plugin_handler_id_str = utility::conversions::to_utf8string(plugin_handler_id.as_string());
		  const std::string& plugin_id_str = utility::conversions::to_utf8string(plugin_id.as_string());
		  const std::string& id_str = utility::conversions::to_utf8string(id.as_string());

          if (id_str.empty() || plugin_id_str.empty() || plugin_handler_id_str.empty()){
            
            // Inform of error: malformed parameters.
            error = default_errors::plugin_malformed_parameters;
            error_description = default_error_descriptions::plugin_malformed_parameters;
          }else{
            
            // use a plug-in handler to remove the plug-in with given id.
            const std::unique_ptr<granada::plugin::PluginHandler>& plugin_handler = plugin_factory()->PluginHandler_unique_ptr(plugin_handler_id_str);
            plugin_handler->Remove(id_str);
          }
        }else{
          
          // Inform of error: malformed parameters.
          error = default_errors::plugin_malformed_parameters;
          error_description = default_error_descriptions::plugin_malformed_parameters;
        }

      }else{

        // Inform of error: missing_parameter
        error = default_errors::plugin_missing_parameter;
        error_description = default_error_descriptions::plugin_missing_parameter;
      }

      if (!error.empty()){

        // respond with a JSON with an error code and an error_description like:
        // { "error" : "missing_parameter", "error_description" : "One or more of the given parameters is missing."}
		  response[utility::conversions::to_string_t(default_strings::plugin_error)] = web::json::value::string(utility::conversions::to_string_t(error));
		  response[utility::conversions::to_string_t(default_strings::plugin_error_description)] = web::json::value::string(utility::conversions::to_string_t(error_description));
      }

      return response;
    }


    void PluginHandler::Fire(const std::string& event_name){
      // build empty parameters.
      web::json::value parameters = web::json::value::object();
      Fire(event_name,parameters,[](const web::json::value& data){},[](const web::json::value& data){});
    }


    void PluginHandler::Fire(const std::string& event_name, web::json::value& parameters){
      Fire(event_name,parameters,[](const web::json::value& data){},[](const web::json::value& data){});
    }


    void PluginHandler::Fire(const std::string& event_name, web::json::value& parameters, function_void_json success, function_void_json failure){

      web::json::value response = web::json::value::object();

      if (event_name.empty()){
		response[utility::conversions::to_string_t(default_strings::plugin_error)] = web::json::value::string(utility::conversions::to_string_t(default_errors::plugin_malformed_parameters));
		response[utility::conversions::to_string_t(default_strings::plugin_error_description)] = web::json::value::string(utility::conversions::to_string_t(default_error_descriptions::plugin_malformed_parameters));
        failure(response);
      }else{
        // lazy load plug-ins
        FireLoadEvent(event_name);

        web::json::value response_data;

        // retrieve all the ids of the plug-ins listening to
        // the fired event.
        const std::string& plugin_ids_str = cache()->Read(plugin_event_value_hash(event_name),entity_keys::plugin_event_ids);

        if (plugin_ids_str.empty()){
          response_data = web::json::value::object();
        }else{

          std::vector<std::string> plugin_ids;
          granada::util::string::split(plugin_ids_str,',',plugin_ids);

          // synchronously run plug-ins
          response_data = Run(plugin_ids,event_name,parameters);

        }

		response[utility::conversions::to_string_t(entity_keys::plugin_parameter_data)] = response_data;
        success(response);
        
      }
    }


    web::json::value PluginHandler::Fire(const web::json::value& parameters){

      web::json::value response = web::json::value::object();
      std::string error;
      std::string error_description;

      // check if all needed parameters are present.
      if (!parameters.is_null()
		  && parameters.has_field(utility::conversions::to_string_t(entity_keys::plugin_parameter_event_name))
		  && parameters.has_field(utility::conversions::to_string_t(entity_keys::plugin_parameters))
		  && parameters.has_field(utility::conversions::to_string_t(entity_keys::plugin_parameter_plugin_handler_id))
		  && parameters.has_field(utility::conversions::to_string_t(entity_keys::plugin_parameter_plugin_id))){

	    const web::json::value event_name = parameters.at(utility::conversions::to_string_t(entity_keys::plugin_parameter_event_name));
		web::json::value fire_parameters = parameters.at(utility::conversions::to_string_t(entity_keys::plugin_parameters));
		const web::json::value plugin_handler_id = parameters.at(utility::conversions::to_string_t(entity_keys::plugin_parameter_plugin_handler_id));
		const web::json::value plugin_id = parameters.at(utility::conversions::to_string_t(entity_keys::plugin_parameter_plugin_id));

        // check if the needed parameters are of the correct type
        if (event_name.is_string()
            && fire_parameters.is_object()
            && plugin_handler_id.is_string()
            && plugin_id.is_string()){
			
		  const std::string& event_name_str = utility::conversions::to_utf8string(event_name.as_string());
		  const std::string& plugin_handler_id_str = utility::conversions::to_utf8string(plugin_handler_id.as_string());
		  const std::string& plugin_id_str = utility::conversions::to_utf8string(plugin_id.as_string());

          if (event_name_str.empty() || plugin_handler_id_str.empty() || plugin_id_str.empty()){

            // Inform of error: malformed parameters.
            error = default_errors::plugin_malformed_parameters;
            error_description = default_error_descriptions::plugin_malformed_parameters;
          }else{
            const std::unique_ptr<granada::plugin::PluginHandler>& plugin_handler = plugin_factory()->PluginHandler_unique_ptr(plugin_handler_id_str);

            plugin_handler->Fire(event_name_str, fire_parameters, [&response](const web::json::value& data){
			  response[utility::conversions::to_string_t(entity_keys::plugin_parameter_data)] = std::move(data);
            }, [&response](const web::json::value& data){
              response = std::move(data);
            });
          }

        }else{
          // Inform of error: malformed parameters.
          error = default_errors::plugin_malformed_parameters;
          error_description = default_error_descriptions::plugin_malformed_parameters;
        }

      }else{

        // Inform of error: missing_parameter
        error = default_errors::plugin_missing_parameter;
        error_description = default_error_descriptions::plugin_missing_parameter;
      }

      if (!error.empty()){

        // respond with a JSON with an error code and an error_description like:
        // { "error" : "missing_parameter", "error_description" : "One or more of the given parameters is missing."}
		response[utility::conversions::to_string_t(default_strings::plugin_error)] = web::json::value::string(utility::conversions::to_string_t(error));
		response[utility::conversions::to_string_t(default_strings::plugin_error_description)] = web::json::value::string(utility::conversions::to_string_t(error_description));
      }

      return response;
    }


    web::json::value PluginHandler::SendMessage(const std::string& from, const web::json::value& to_ids, const web::json::value& message_data){
      
      const bool& malformed_parameters = from.empty() || !to_ids.is_array() || message_data.is_null();

      web::json::value response = web::json::value::object();
      std::string error;
      std::string error_description;

      if (malformed_parameters){

        // Inform of error: malformed parameters.
        error = default_errors::plugin_malformed_parameters;
        error_description = default_error_descriptions::plugin_malformed_parameters;
      }else{

        // JSON object containing all the plugin responses to the message.
        web::json::value message_responses;

        // if destination ids array is empty, then we send message to all plugins.
        // if destination id is specified, only send message to specified plugins.
        std::vector<std::string> destination_ids;
        
        if (to_ids.size() == 0){

          // destination ids are not specified, send message to ALL plug-ins except the sender.

          const std::unique_ptr<granada::cache::CacheHandlerIterator>& cache_iterator = cache()->make_iterator(plugin_value_hash("*"));
          int i = 0;
          while (cache_iterator->has_next()){

            const std::string& plugin_hash = cache_iterator->next();
            const std::string& extended = cache()->Read(plugin_hash,entity_keys::plugin_extended);

            // only send messages to active plug-ins, plug-ins that haven't been extended.
            if (extended!=default_strings::plugin_extended_true){

              const std::string& destination_plugin_id = cache()->Read(plugin_hash,entity_keys::plugin_header_id);
              
              // do not send message to the entity that is writing it.
              if (!destination_plugin_id.empty() && destination_plugin_id != from){
                destination_ids.push_back(destination_plugin_id);
                i++;

                // group plug-ins so it consumes less.

                if (i==PluginHandler::SEND_MESSAGE_PLUGIN_GROUP_SIZE_){
                  
                  // group is completed, send message to all the plug-ins of the group
                  // in one time.
                  const web::json::value& partial_message_responses = SendMessage(from,destination_ids,message_data);
                  for (auto it = partial_message_responses.as_object().cbegin(); it != partial_message_responses.as_object().cend(); ++it){
                    message_responses[it->first] = it->second;
                  }
                  destination_ids.clear();
                  i = 0;
                }
              }
            }
          }

          // process an incomplete group of plug-ins, send message to these plug-ins
          // in one time.
          if (destination_ids.size() > 0 && destination_ids.size() < PluginHandler::SEND_MESSAGE_PLUGIN_GROUP_SIZE_){
            const web::json::value& partial_message_responses = SendMessage(from,destination_ids,message_data);
            for (auto it = partial_message_responses.as_object().cbegin(); it != partial_message_responses.as_object().cend(); ++it){
              message_responses[it->first] = it->second;
            }
          }
        }else{

          // destination ids are specified, send message ONLY to specified plugins.
          for(auto it = to_ids.as_array().cbegin(); it != to_ids.as_array().cend(); ++it){
            if (it->is_string()){
				destination_ids.push_back(utility::conversions::to_utf8string(it->as_string()));
            }
          }
          message_responses = SendMessage(from,destination_ids,message_data);
        }

        // if there is no error respond with a JSON with the value.
		response[utility::conversions::to_string_t(entity_keys::plugin_parameter_data)] = message_responses;

      }


      if (!error.empty()){

        // respond with a JSON with an error code and an error_description like:
        // { "error" : "missing_parameter", "error_description" : "One or more of the given parameters is missing."}
		response[utility::conversions::to_string_t(default_strings::plugin_error)] = web::json::value::string(utility::conversions::to_string_t(error));
		response[utility::conversions::to_string_t(default_strings::plugin_error_description)] = web::json::value::string(utility::conversions::to_string_t(error_description));
      }
      
      return response;
    }


    web::json::value PluginHandler::SendMessage(const web::json::value& parameters){

      web::json::value response;
      std::string error;
      std::string error_description;

      // check if all needed parameters are present.
      if (!parameters.is_null()
		  && parameters.has_field(utility::conversions::to_string_t(entity_keys::plugin_parameter_to_ids))
		  && parameters.has_field(utility::conversions::to_string_t(entity_keys::plugin_parameters))
		  && parameters.has_field(utility::conversions::to_string_t(entity_keys::plugin_parameter_plugin_handler_id))
		  && parameters.has_field(utility::conversions::to_string_t(entity_keys::plugin_parameter_plugin_id))){

		const web::json::value& to_ids = parameters.at(utility::conversions::to_string_t(entity_keys::plugin_parameter_to_ids));
		web::json::value message_parameters = parameters.at(utility::conversions::to_string_t(entity_keys::plugin_parameters));
		const web::json::value& plugin_handler_id = parameters.at(utility::conversions::to_string_t(entity_keys::plugin_parameter_plugin_handler_id));
		const web::json::value& plugin_id = parameters.at(utility::conversions::to_string_t(entity_keys::plugin_parameter_plugin_id));

        // check if the needed parameters are of the correct type
        if (to_ids.is_array()
            && message_parameters.is_object()
            && plugin_handler_id.is_string()
            && plugin_id.is_string()){

	      const std::string& plugin_handler_id_str = utility::conversions::to_utf8string(plugin_handler_id.as_string());
		  const std::string& plugin_id_str = utility::conversions::to_utf8string(plugin_id.as_string());

          if (plugin_handler_id_str.empty() || plugin_id_str.empty()){

            // Inform of error: malformed parameters.
            error = default_errors::plugin_malformed_parameters;
            error_description = default_error_descriptions::plugin_malformed_parameters;
          }else{
            const std::unique_ptr<granada::plugin::PluginHandler>& plugin_handler = plugin_factory()->PluginHandler_unique_ptr(plugin_handler_id_str);
            response = plugin_handler->SendMessage(plugin_id_str,to_ids,message_parameters);
          }


        }else{
          
          // Inform of error: malformed parameters.
          error = default_errors::plugin_malformed_parameters;
          error_description = default_error_descriptions::plugin_malformed_parameters;
        }

      }else{

        // Inform of error: missing_parameter
        error = default_errors::plugin_missing_parameter;
        error_description = default_error_descriptions::plugin_missing_parameter;
      }

      if (!error.empty()){
        // respond with a JSON with an error code and an error_description like:
        // { "error" : "missing_parameter", "error_description" : "One or more of the given parameters is missing."}
		response[utility::conversions::to_string_t(default_strings::plugin_error)] = web::json::value::string(utility::conversions::to_string_t(error));
		response[utility::conversions::to_string_t(default_strings::plugin_error_description)] = web::json::value::string(utility::conversions::to_string_t(error_description));
      }

      return response;
    }


    web::json::value PluginHandler::SetValue(const web::json::value& parameters){

      std::string error;
      std::string error_description;

      // check if all needed parameters are present.
      if (!parameters.is_null()
		  && parameters.has_field(utility::conversions::to_string_t(entity_keys::plugin_parameter_key))
		  && parameters.has_field(utility::conversions::to_string_t(entity_keys::plugin_parameter_value))
		  && parameters.has_field(utility::conversions::to_string_t(entity_keys::plugin_parameter_plugin_handler_id))
		  && parameters.has_field(utility::conversions::to_string_t(entity_keys::plugin_parameter_plugin_id))){

		const web::json::value& key = parameters.at(utility::conversions::to_string_t(entity_keys::plugin_parameter_key));
		const web::json::value& value = parameters.at(utility::conversions::to_string_t(entity_keys::plugin_parameter_value));
		const web::json::value& plugin_handler_id = parameters.at(utility::conversions::to_string_t(entity_keys::plugin_parameter_plugin_handler_id));
		const web::json::value& plugin_id = parameters.at(utility::conversions::to_string_t(entity_keys::plugin_parameter_plugin_id));

        // check all the needed parameters are of the correct type.
        if (key.is_string()
            && value.is_string()
            && plugin_handler_id.is_string()
            && plugin_id.is_string()){

		  const std::string& plugin_handler_id_str = utility::conversions::to_utf8string(plugin_handler_id.as_string());
		  const std::string& plugin_id_str = utility::conversions::to_utf8string(plugin_id.as_string());
		  const std::string& key_str = utility::conversions::to_utf8string(key.as_string());
		  const std::string& value_str = utility::conversions::to_utf8string(value.as_string());

          if (key_str.empty()
              || value_str.empty()
              || plugin_handler_id_str.empty()
              || plugin_id_str.empty()){
          
            // Inform of error: malformed parameters.
            error = default_errors::plugin_malformed_parameters;
            error_description = default_error_descriptions::plugin_malformed_parameters;
          }else{

            // store value in the cache. It will be stored until the plug-in is removed or the Plug-in Handler is stopped.
            cache()->Write(plugin_store_hash(plugin_handler_id_str,plugin_id_str),key_str,value_str);
          }

        }else{

          // Inform of error: malformed parameters.
          error = default_errors::plugin_malformed_parameters;
          error_description = default_error_descriptions::plugin_malformed_parameters;
        }
      }else{

        // Inform of error: missing_parameter
        error = default_errors::plugin_missing_parameter;
        error_description = default_error_descriptions::plugin_missing_parameter;
      }

      web::json::value response = web::json::value::object();
      
      if (!error.empty()){

        // respond with a JSON with an error code and an error_description like:
        // { "error" : "missing_parameter", "error_description" : "One or more of the given parameters is missing."}
		response[utility::conversions::to_string_t(default_strings::plugin_error)] = web::json::value::string(utility::conversions::to_string_t(error));
		response[utility::conversions::to_string_t(default_strings::plugin_error_description)] = web::json::value::string(utility::conversions::to_string_t(error_description));
      }

      // if there is no error, respond with an empty JSON object {}

      return response;
    }



    web::json::value PluginHandler::GetValue(const web::json::value& parameters){

      std::string error;
      std::string error_description;
      std::string value;

      // check if all needed parameters are present.
      if (!parameters.is_null()
		  && parameters.has_field(utility::conversions::to_string_t(entity_keys::plugin_parameter_key))
		  && parameters.has_field(utility::conversions::to_string_t(entity_keys::plugin_parameter_plugin_handler_id))
		  && parameters.has_field(utility::conversions::to_string_t(entity_keys::plugin_parameter_plugin_id))){

		const web::json::value& key = parameters.at(utility::conversions::to_string_t(entity_keys::plugin_parameter_key));
		const web::json::value& plugin_handler_id = parameters.at(utility::conversions::to_string_t(entity_keys::plugin_parameter_plugin_handler_id));
		const web::json::value& plugin_id = parameters.at(utility::conversions::to_string_t(entity_keys::plugin_parameter_plugin_id));

        // check all the needed parameters are of the correct type.
        if (key.is_string()
            && plugin_handler_id.is_string()
            && plugin_id.is_string()){

	      const std::string& key_str = utility::conversions::to_utf8string(key.as_string());
		  const std::string& plugin_handler_id_str = utility::conversions::to_utf8string(plugin_handler_id.as_string());
		  const std::string& plugin_id_str = utility::conversions::to_utf8string(plugin_id.as_string());

          if (key_str.empty() || plugin_handler_id_str.empty() || plugin_id_str.empty()){

            // Inform of error: malformed parameters.
            error = default_errors::plugin_malformed_parameters;
            error_description = default_error_descriptions::plugin_malformed_parameters;
          }else{            

            // read value stored in the cache, if value does not exists, we will get an empty string.
            value = cache()->Read(plugin_store_hash(plugin_handler_id_str,plugin_id_str), key_str);
          }
        }else{

          // Inform of error: malformed parameters.
          error = default_errors::plugin_malformed_parameters;
          error_description = default_error_descriptions::plugin_malformed_parameters;
        }
      }else{

        // Inform of error: missing_parameter
        error = default_errors::plugin_missing_parameter;
        error_description = default_error_descriptions::plugin_missing_parameter;
      }
      
      web::json::value response = web::json::value::object();

      if (error.empty()){
        // if there is no error respond with a JSON with the value.
		  response[utility::conversions::to_string_t(default_strings::plugin_value)] = web::json::value::string(utility::conversions::to_string_t(value));
      }else{

        // respond with a JSON with an error code and an error_description like:
        // { "error" : "missing_parameter", "error_description" : "One or more of the given parameters is missing."}
		  response[utility::conversions::to_string_t(default_strings::plugin_error)] = web::json::value::string(utility::conversions::to_string_t(error));
		  response[utility::conversions::to_string_t(default_strings::plugin_error_description)] = web::json::value::string(utility::conversions::to_string_t(error_description));
      }

      return response;
    }


    web::json::value PluginHandler::DestroyValue(const web::json::value& parameters){

      std::string error;
      std::string error_description;

      // check if all needed parameters are present.
      if (!parameters.is_null()
		  && parameters.has_field(utility::conversions::to_string_t(entity_keys::plugin_parameter_key))
		  && parameters.has_field(utility::conversions::to_string_t(entity_keys::plugin_parameter_plugin_handler_id))
		  && parameters.has_field(utility::conversions::to_string_t(entity_keys::plugin_parameter_plugin_id))){

		const web::json::value& key = parameters.at(utility::conversions::to_string_t(entity_keys::plugin_parameter_key));
		const web::json::value& plugin_handler_id = parameters.at(utility::conversions::to_string_t(entity_keys::plugin_parameter_plugin_handler_id));
		const web::json::value& plugin_id = parameters.at(utility::conversions::to_string_t(entity_keys::plugin_parameter_plugin_id));

        // check all the needed parameters are of the correct type.
        if (key.is_string()
            && plugin_handler_id.is_string()
            && plugin_id.is_string()){

		  const std::string& key_str = utility::conversions::to_utf8string(key.as_string());
		  const std::string& plugin_handler_id_str = utility::conversions::to_utf8string(plugin_handler_id.as_string());
		  const std::string& plugin_id_str = utility::conversions::to_utf8string(plugin_id.as_string());

          if (key_str.empty() || plugin_handler_id_str.empty() || plugin_id_str.empty()){

            // Inform of error: malformed parameters.
            error = default_errors::plugin_malformed_parameters;
            error_description = default_error_descriptions::plugin_malformed_parameters;
          }else{

            // destroy the value stored in the cache.
            cache()->Destroy(plugin_store_hash(plugin_handler_id_str,plugin_id_str), key_str);
          }
        }else{

          // Inform of error: malformed parameters.
          error = default_errors::plugin_malformed_parameters;
          error_description = default_error_descriptions::plugin_malformed_parameters;
        }
      }else{

        // Inform of error: missing_parameter
        error = default_errors::plugin_missing_parameter;
        error_description = default_error_descriptions::plugin_missing_parameter;
      }
      
      web::json::value response = web::json::value::object();

      if (!error.empty()){
        // respond with a JSON with an error code and an error_description like:
        // { "error" : "missing_parameter", "error_description" : "One or more of the given parameters is missing."}
		  response[utility::conversions::to_string_t(default_strings::plugin_error)] = web::json::value::string(utility::conversions::to_string_t(error));
		  response[utility::conversions::to_string_t(default_strings::plugin_error_description)] = web::json::value::string(utility::conversions::to_string_t(error_description));
      }

      return response;
    }


    web::json::value PluginHandler::ClearValues(const web::json::value& parameters){

      std::string error;
      std::string error_description;

      // check if all needed parameters are present.
      if (!parameters.is_null()
		  && parameters.has_field(utility::conversions::to_string_t(entity_keys::plugin_parameter_plugin_handler_id))
		  && parameters.has_field(utility::conversions::to_string_t(entity_keys::plugin_parameter_plugin_id))){

		  const web::json::value& plugin_handler_id = parameters.at(utility::conversions::to_string_t(entity_keys::plugin_parameter_plugin_handler_id));
		  const web::json::value& plugin_id = parameters.at(utility::conversions::to_string_t(entity_keys::plugin_parameter_plugin_id));

        // check all the needed parameters are of the correct type.
        if (plugin_handler_id.is_string()
            && plugin_id.is_string()){

			const std::string& plugin_handler_id_str = utility::conversions::to_utf8string(plugin_handler_id.as_string());
			const std::string& plugin_id_str = utility::conversions::to_utf8string(plugin_id.as_string());

          if (plugin_handler_id_str.empty() || plugin_id_str.empty()){

            // Inform of error: malformed parameters.
            error = default_errors::plugin_malformed_parameters;
            error_description = default_error_descriptions::plugin_malformed_parameters;
          }else{

            // destroy the value stored in the cache.
            cache()->Destroy(plugin_store_hash(plugin_handler_id_str,plugin_id_str));
          }

        }else{

          // Inform of error: malformed parameters.
          error = default_errors::plugin_malformed_parameters;
          error_description = default_error_descriptions::plugin_malformed_parameters;
        }
      }else{

        // Inform of error: missing_parameter
        error = default_errors::plugin_missing_parameter;
        error_description = default_error_descriptions::plugin_missing_parameter;
      }
      
      web::json::value response = web::json::value::object();

      if (!error.empty()){
        // respond with a JSON with an error code and an error_description like:
        // { "error" : "missing_parameter", "error_description" : "One or more of the given parameters is missing."}
		  response[utility::conversions::to_string_t(default_strings::plugin_error)] = web::json::value::string(utility::conversions::to_string_t(error));
		  response[utility::conversions::to_string_t(default_strings::plugin_error_description)] = web::json::value::string(utility::conversions::to_string_t(error_description));
      }

      return response;
    }


    void PluginHandler::RunnerLock(){

      if (PluginHandler::RUNNER_USE_FREQUENCY_LIMIT_>0){

        // retrieve the runner last use from the cache.
        const std::string& plugin_handler_value_hash_str = plugin_handler_value_hash();
        const std::string& t_str = cache()->Read(plugin_handler_value_hash_str,entity_keys::plugin_handler_runner_last_use);

        bool cache_time = true;

        if (!t_str.empty()){
          try{
            const int& milliseconds_to_limit = PluginHandler::RUNNER_USE_FREQUENCY_LIMIT_ - granada::util::time::get_milliseconds_span(std::stoi(t_str));
            if (milliseconds_to_limit > 0){

              // not enough time has elapsed for using
              // the runner, sleep. 
              cache_time = false;
              granada::util::time::sleep_milliseconds(milliseconds_to_limit+1);

              // verify again.
              RunnerLock();

            }
          }catch(const std::logic_error e){}
        }

        if (cache_time){

          // save last use as the time now.
          cache()->Write(plugin_handler_value_hash_str,entity_keys::plugin_handler_runner_last_use,granada::util::time::stringify(granada::util::time::get_milliseconds()));
        }
      }
    }


    void PluginHandler::LoadProperties(){

      // Maximum bytes a Plug-in Handler can load.
      // if value is not in the server.conf file
      // take value from defaults: default_numbers::plugin_bytes_limit
      const std::string& plugin_bytes_limit_str = granada::util::application::GetProperty(entity_keys::plugin_bytes_limit);
      if (plugin_bytes_limit_str.empty()){
        PluginHandler::PLUGIN_BYTES_LIMIT_ = default_numbers::plugin_bytes_limit;
      }else{
        try{
          PluginHandler::PLUGIN_BYTES_LIMIT_ = std::stoi(plugin_bytes_limit_str);
        }catch(const std::logic_error e){
          PluginHandler::PLUGIN_BYTES_LIMIT_ = default_numbers::plugin_bytes_limit;
        }
      }

      // The minimum time Plug-ins have to wait
      // between two runs in milliseconds.
      const std::string& runner_use_frequency_limit_str = granada::util::application::GetProperty(entity_keys::plugin_runner_use_frequency_limit);
      if (runner_use_frequency_limit_str.empty()){
        PluginHandler::RUNNER_USE_FREQUENCY_LIMIT_ = default_numbers::plugin_runner_use_frequency_limit;
      }else{
        try{
          PluginHandler::RUNNER_USE_FREQUENCY_LIMIT_ = std::stoi(runner_use_frequency_limit_str);
        }catch(const std::logic_error e){
          PluginHandler::RUNNER_USE_FREQUENCY_LIMIT_ = default_numbers::plugin_runner_use_frequency_limit;
        }
      }
    }


    std::string PluginHandler::GetUID(){
      std::stringstream ss;
      PluginHandler::uid_mtx_.lock();
      PluginHandler::uid_++;
      ss << PluginHandler::uid_;
      PluginHandler::uid_mtx_.unlock();
      return ss.str();
    }


    int PluginHandler::PreloadRepositories(){

      // cumulated loaded repositories
      // size in bytes.
      int total_size = 0;

      // loop through all given packaged plugins "repositories"
      std::string repository_path;
      for (auto it = paths_.begin(); it != paths_.end(); ++it){

        repository_path = *it;
        if (!repository_path.empty()){

          // format repository path to homogenize them.
          repository_path = granada::util::application::FormatDirectoryPath(repository_path);
          *it = repository_path;

          if (boost::filesystem::exists(repository_path) && boost::filesystem::is_directory(repository_path)){

            // add all the packaged plug-ins of the repository.
            const int& repository_size = PreloadPackages(repository_path);

            // add the loaded package size to the
            // cumulated size, and if cumulated size
            // is bigger than the maximum bytes 
            // a Plug-in Handler can load then stop
            // preloading repositories and return.
            if (repository_size>0){
              total_size += repository_size;
              if (total_size > PluginHandler::PLUGIN_BYTES_LIMIT_){
                return total_size;
              }
            }
          }
        }
      }
      return total_size;
    }


    int PluginHandler::PreloadPackages(const std::string& repository_path){

      // cumulated loaded package
      // size in bytes.
      int total_size = 0;

      boost::filesystem::directory_iterator end_it;
      boost::filesystem::path package_directory_path;

      // loop through all the plug-ins in a package and cache them.
      for ( boost::filesystem::directory_iterator it2(repository_path); it2 != end_it; ++it2 ){

        package_directory_path = it2->path();
        if ( boost::filesystem::is_directory(it2->status()) ){

          // add the plug-ins of the packages.
          const int& packaged_plugins_size = PreloadPackagedPlugins(package_directory_path.string());

          // add the loaded package size to the
          // cumulated size, and if cumulated size
          // is bigger than the maximum bytes 
          // a Plug-in Handler can load then stop
          // preloading packaged plug-ins and return.
          if (packaged_plugins_size>0){
            total_size += packaged_plugins_size;
            if (total_size > PluginHandler::PLUGIN_BYTES_LIMIT_){
              return total_size;
            }
          }
        }
      }
      return total_size;
    }


    int PluginHandler::PreloadPackagedPlugins(const std::string& package_directory_path){

      // cumulated loaded plug-ins
      // size in bytes.
      int total_size = 0;

      // check if the plugin is a valid server plugin.
      // check if all needed the plugin files are there.

      // check if there is a server folder
      // or a client folder
      const std::string& package_server_directory_path = package_directory_path + "/" + default_strings::plugin_server_directory_name;

      if (boost::filesystem::exists(package_server_directory_path) && boost::filesystem::is_directory(package_server_directory_path)){

        // retrieve the configuration file content, if the file does not exist or does not contain a valid JSON
        // set configuration by default as a stringified empty JSON object {}
        const std::string& configuration_path = package_directory_path + "/" + default_strings::plugin_configuration_file_name + "." + default_strings::plugin_configuration_file_extension;
        
        std::string configuration;
        if (boost::filesystem::exists(configuration_path) && !boost::filesystem::is_directory(configuration_path)){
          const int& configuration_file_size = boost::filesystem::file_size(configuration_path);

          // add the plug-in configuration file size to the
          // cumulated size, and if cumulated size
          // is bigger than the maximum bytes 
          // a Plug-in Handler can load then stop
          // preloading plug-ins and return.
          if (configuration_file_size > 0){
            total_size += configuration_file_size;
            if (total_size > PluginHandler::PLUGIN_BYTES_LIMIT_){
              return total_size;
            }
          }

          configuration = TransformPluginConfigurationPath(configuration_path);
        }



        if (boost::filesystem::exists(package_server_directory_path) && boost::filesystem::is_directory(package_server_directory_path)){
          // get the extensions of the script/executable files
          const std::vector<std::string>& extensions = runner()->extensions();

          boost::filesystem::directory_iterator end_it;

          // loop through the server files of the package, containing the plug-ins scripts/executables and
          // the headers of the plug-ins (id,events,inheritance)
          // Both header and script/executable files have the same name with different extensions:
          // - the header is a .json file
          // - the script/executable has an extension (for example: .js for JavaScript, .sh for shell script, nothing or .exe for executables).
          for ( boost::filesystem::directory_iterator it(package_server_directory_path); it != end_it; ++it ){

            if ( !boost::filesystem::is_directory(it->status()) ){

              std::string plugin_filename = it->path().filename().string();
              const std::string& extension = granada::util::file::GetExtension(plugin_filename);
              if (extension==default_strings::plugin_header_file_extension){

                // check if there is an homologous script/executable file with the extensions and if so add the plugin
                for (auto it = extensions.begin(); it != extensions.end(); ++it){
                  
                  const std::string& script_file_extension = *it;
                  std::string plugin_script_path = package_server_directory_path + "/" + plugin_filename.substr(0, plugin_filename.find_last_of("."));
                  if (!script_file_extension.empty()){
                    plugin_script_path += "." + script_file_extension;
                  }

                  const std::string& header_path = package_server_directory_path + "/" + plugin_filename;
                  const int& header_file_size = boost::filesystem::file_size(header_path);

                  // add the plug-in header file size to the
                  // cumulated size, and if cumulated size
                  // is bigger than the maximum bytes 
                  // a Plug-in Handler can load then stop
                  // preloading plug-ins and return.
                  if (header_file_size > 0){
                    total_size += header_file_size;
                    if (total_size > PluginHandler::PLUGIN_BYTES_LIMIT_){
                      return total_size;
                    }
                  }
                  
                  // script/executable file and header have to exist both
                  // check if script/executable file exists
                  if (boost::filesystem::exists(plugin_script_path)){

                    const std::string& header_str = granada::util::file::ContentAsString(header_path);

                    // Cache the plug-in using a cache driver so it can quickly be used when called or when one
                    // of the events the plug-in is listening is fired.
                    web::json::value header = granada::util::string::to_json(header_str);

                    // check if plug-in is active or not,
                    // only preload plug-in if it is active.
                    bool active = true;
					if (header.has_field(utility::conversions::to_string_t(entity_keys::plugin_header_active))){
					  const web::json::value& active_json = header.at(utility::conversions::to_string_t(entity_keys::plugin_header_active));
                      if (active_json.is_boolean()){
                        active = active_json.as_bool();
                      }
                    }

                    if (active){
                      const int& script_file_size = boost::filesystem::file_size(plugin_script_path);

                      // add the plug-in script file size to the
                      // cumulated size, and if cumulated size
                      // is bigger than the maximum bytes 
                      // a Plug-in Handler can load then stop
                      // preloading plug-ins and return.
                      if (script_file_size > 0){
                        total_size += script_file_size;
                        if (total_size > PluginHandler::PLUGIN_BYTES_LIMIT_){
                          return total_size;
                        }
                      }
                      const std::string& script = TransformPluginScriptPath(plugin_script_path);
                      AddPluginLoader(header, configuration, script);
                      break;
                    }
                  }
                }
              }
            }
          }
        }
      }

      return total_size;
    }


    std::string PluginHandler::TransformPluginScriptPath(const std::string& plugin_script_path){
      return plugin_script_path;
    }


    std::string PluginHandler::TransformPluginConfigurationPath(const std::string& plugin_configuration_path){
      return plugin_configuration_path;
    }


    void PluginHandler::FireLoadEvent(const std::string& event_name){

      // retrieve the plug-in loaders that have to be processed for
      // the given event.
      const web::json::value& event_loaders = granada::util::string::to_json(cache()->Read(plugin_event_value_hash(event_name),entity_keys::plugin_event_loader));

      // loop through the plug-in loaders and add the plug-ins if they are
      // not already added.
      for(auto it = event_loaders.as_object().cbegin(); it != event_loaders.as_object().cend(); ++it){
		  const std::string& plugin_id = utility::conversions::to_utf8string(it->first);
        const std::unique_ptr<granada::plugin::Plugin>& plugin = plugin_factory()->Plugin_unique_ptr(this,plugin_id);
        if (!plugin->Exists() && Load(plugin.get(),it->second)){

          // add plug-in, but tell to not run plug-in
          // even if it does not have "run" events.
          plugin->IsRunnable(false);
          Add(plugin.get());
          // remove plug-in loader to prevent plug-in from being added.
          RemovePluginLoader(plugin_id);
        }
      }
      RemoveLoadEvent(event_name);
    }


    void PluginHandler::AddFunctionsToRunner(){

      // add c++ functions so they can be called in the script or executable.
      const std::shared_ptr<granada::plugin::PluginHandler>& plugin_handler = plugin_factory()->PluginHandler_unique_ptr();

      plugin_handler->runner()->functions()->Add(entity_keys::plugin_script_function_send_message, [plugin_handler](const web::json::value& parameters){
        return plugin_handler->SendMessage(parameters);
      });

      plugin_handler->runner()->functions()->Add(entity_keys::plugin_script_function_set_value, [plugin_handler](const web::json::value& parameters){
        return plugin_handler->SetValue(parameters);
      });

      plugin_handler->runner()->functions()->Add(entity_keys::plugin_script_function_get_value, [plugin_handler](const web::json::value& parameters){
        return plugin_handler->GetValue(parameters);
      });

      plugin_handler->runner()->functions()->Add(entity_keys::plugin_script_function_destroy_value, [plugin_handler](const web::json::value& parameters){
        return plugin_handler->DestroyValue(parameters);
      });

      plugin_handler->runner()->functions()->Add(entity_keys::plugin_script_function_clear_values, [plugin_handler](const web::json::value& parameters){
        return plugin_handler->ClearValues(parameters);
      });

      plugin_handler->runner()->functions()->Add(entity_keys::plugin_script_function_fire, [plugin_handler](const web::json::value& parameters){
        return plugin_handler->Fire(parameters);
      });

      plugin_handler->runner()->functions()->Add(entity_keys::plugin_script_function_run_plugin, [plugin_handler](const web::json::value& parameters){
        return plugin_handler->Run(parameters);
      });

      plugin_handler->runner()->functions()->Add(entity_keys::plugin_script_function_remove, [plugin_handler](const web::json::value& parameters){
        return plugin_handler->Remove(parameters);
      });

      plugin_handler->runner()->functions()->Add(entity_keys::plugin_script_function_remove_events, [plugin_handler](const web::json::value& parameters){
        return plugin_handler->RemoveEventListeners(parameters);
      });

    }


    granada::util::mutex::call_once Plugin::load_properties_call_once_;


    bool Plugin::Exists(){
      return plugin_handler()->cache()->Exists(plugin_handler()->plugin_value_hash(id_),entity_keys::plugin_header_id);
    }


    void Plugin::IsExtended(bool extended){
      if (extended){
        plugin_handler()->cache()->Write(plugin_handler()->plugin_value_hash(id_),entity_keys::plugin_extended,default_strings::plugin_extended_true);
        
        // remove plug-in from events lists so it cannot run again.
        const web::json::value& events = granada::util::json::as_array(header_,entity_keys::plugin_header_events);
        for (auto it = events.as_array().cbegin(); it != events.as_array().cend(); ++it){
          if (it->is_string()){
			  plugin_handler()->RemoveEventListener(utility::conversions::to_utf8string(it->as_string()), id_);
          }
        }
      }else{
        plugin_handler()->cache()->Destroy(plugin_handler()->plugin_value_hash(id_),entity_keys::plugin_extended);
        
        // add plugin to event list so it can run.
        const web::json::value& events = granada::util::json::as_array(header_,entity_keys::plugin_header_events);
        for (auto it = events.as_array().cbegin(); it != events.as_array().cend(); ++it){
          if (it->is_string()){
			  plugin_handler()->AddEventListener(utility::conversions::to_utf8string(it->as_string()), id_);
          }
        }
      }
    }


    const bool Plugin::IsExtended(){
      if (plugin_handler()->cache()->Read(plugin_handler()->plugin_value_hash(id_),entity_keys::plugin_extended) == default_strings::plugin_extended_true){
        return true;
      }
      return false;
    }

  }
}
