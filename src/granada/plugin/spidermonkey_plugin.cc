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
  */

#include "granada/plugin/spidermonkey_plugin.h"

namespace granada{

  namespace plugin{


    void SpidermonkeyPluginHandler::Extend(const std::shared_ptr<granada::plugin::Plugin>& plugin){
      const web::json::value& extends = granada::util::json::as_array(plugin->GetHeader(),entity_keys::plugin_header_extends);

      std::vector<std::shared_ptr<granada::plugin::Plugin>> extended_plugins;
      for(auto it = extends.as_array().cbegin(); it != extends.as_array().cend(); ++it){
        if (it->is_string()){
          const std::shared_ptr<granada::plugin::Plugin>& extended_plugin = GetPluginById(it->as_string());
          extended_plugins.push_back(extended_plugin);
        }
      }

      Extend(extended_plugins,plugin);

    }


    void SpidermonkeyPluginHandler::Extend(const std::shared_ptr<granada::plugin::Plugin>& extended_plugin, const std::shared_ptr<granada::plugin::Plugin>& plugin){
      std::vector<std::shared_ptr<granada::plugin::Plugin>> extended_plugins;
      extended_plugins.push_back(extended_plugin);
      Extend(extended_plugins,plugin);
    }


    void SpidermonkeyPluginHandler::Extend(const std::vector<std::shared_ptr<granada::plugin::Plugin>>& extended_plugins, const std::shared_ptr<granada::plugin::Plugin>& plugin){
      if (plugin.get()!=nullptr){
        std::string extended_plugins_scripts = "";
        std::string extended_configurations = "";

        int i=0;
        for (auto it = extended_plugins.begin(); it != extended_plugins.end(); ++it){
          const std::shared_ptr<granada::plugin::Plugin>& extended_plugin = *it;

          // check that plug-in has been added yet.
          if (extended_plugin.get()==nullptr){

            // the plug-in that has to be extended has not been added yet.
            // its extension will be applied when it will be added.
            AddExtension(extended_plugin->GetId(),plugin->GetId());

          }else{

            if (i>0){
              extended_plugins_scripts += ",";
              extended_configurations += ",";
            }else{
              i++;
            }

            extended_plugins_scripts += "\"" + extended_plugin->GetId() + "\":" + extended_plugin->GetScript();
            extended_configurations += "\"" + extended_plugin->GetId() + "\":" + extended_plugin->GetConfiguration().serialize();
            
            // tag plug-in as extended so it is no more runnable.
            extended_plugin->IsExtended(true);
            
          }
        }

        std::string script = plugin->GetScript();
        if (!extended_plugins_scripts.empty() && !script.empty()){
          extended_plugins_scripts = "{" + extended_plugins_scripts + "}";
          extended_configurations = "{" + extended_configurations + "}";

          {
            script = GetJavaScriptPluginExtension(plugin) + " var __EXTENDED_PLUGINS = " + extended_plugins_scripts + "; var __EXTENDED_CONFIGURATIONS = " + extended_configurations + "; var __PLUGIN_JSON = " + script  + "; __extend(); __print(__PLUGIN_JSON);";
          }
          
          // wait until runner is usable, it is recommended to 
          // limit the use of the runner so it does not harm
          // other users performance.
          RunnerLock();

          const std::string& response = runner()->Run(script);
          plugin->SetScript(response);

          // store the new plug-in script value in the cache.
          cache()->Write(plugin_value_hash(plugin->GetId()),entity_keys::plugin_script,response);
        }
      }

    }


    void SpidermonkeyPluginHandler::Run(const std::shared_ptr<granada::plugin::Plugin>& plugin, web::json::value& parameters,const std::string& event_name, function_void_json success, function_void_json failure){

      // run plug-in and return response.
        
      std::string script = plugin->GetScript();
      if (script.empty()){

        // return an empty json object as response.
        web::json::value response = web::json::value::object();
        response[entity_keys::plugin_parameter_data] = web::json::value::object();
        success(response);
      }else{

        // build the script to execute.
        {
            script =  GetJavaScriptPluginCore(plugin) + " var __PLUGIN = " + script  + "; __wrappedRun(" + granada::util::string::stringified_json(parameters.serialize());
        }
        
        if (event_name.empty()){
          script += ",null);";
        }else{
          script += ",\"" + event_name + "\");";
        }

        // wait until runner is usable, it is recommended to 
        // limit the use of the runner so it does not harm
        // other users performance.
        RunnerLock();

        // run the script and parse response to json.
        web::json::value response_data = granada::util::string::to_json(runner()->Run(script));

        if (response_data.has_field(default_strings::plugin_error)){

          // if response contains error, call failure callback and
          // fire plug-in run failure event and remove plug-in
          // so it cannot be called again.

          response_data[entity_keys::plugin_id] = web::json::value::string(plugin->GetId());
          failure(response_data);
          const std::string& plugin_run_failure_after_event = default_strings::plugin_run_failure_event + "-" + default_strings::plugin_after;
          PluginHandler::Fire(plugin_run_failure_after_event,response_data);
          PluginHandler::Fire(plugin->GetId() + "-" + plugin_run_failure_after_event,response_data);
          
          // Managing plug-in execution failure
          // if it is a script_error remove the plug-in so it won't be able to be executed again.
          web::json::value error = response_data.at(default_strings::plugin_error);
          if (error.is_string() && error.as_string() == default_errors::runner_script_error){
            Remove(plugin->GetId());
          }

        }else{

          web::json::value response = web::json::value::object();
          response[entity_keys::plugin_parameter_data] = std::move(response_data);
          success(response);
        }

      }

    }


    web::json::value SpidermonkeyPluginHandler::Run(const std::vector<std::string>& plugin_ids, const std::string& event_name, web::json::value& parameters){

      std::string script = MultiplePluginScript(plugin_ids);

      if (script.empty()){
        web::json::value response = web::json::value::object();
        response[default_strings::plugin_error] = web::json::value::string(default_errors::plugin_empty_script);
        return response;
      }else{

        // cache script so it can be reused
        cache()->Write(plugin_event_value_hash(event_name),entity_keys::plugin_event_script,script);  
        return Run(script,event_name,parameters);
      }

    }


    void SpidermonkeyPluginHandler::Fire(const std::string& event_name, web::json::value& parameters, function_void_json success, function_void_json failure){

      web::json::value response = web::json::value::object();
      
      if (event_name.empty()){

        // inform of malformed parameters.
        response[default_strings::plugin_error] = web::json::value::string(default_errors::plugin_malformed_parameters);
        response[default_strings::plugin_error_description] = web::json::value::string(default_error_descriptions::plugin_malformed_parameters);
        failure(response);
      }else{

        FireLoadEvent(event_name);

        web::json::value response_data = web::json::value::object();
        
        const std::string& event_value_hash = plugin_event_value_hash(event_name);

        // check if a script containing the plug-ins
        // is already cached, if so use it.
        std::string script = cache()->Read(event_value_hash,entity_keys::plugin_event_script);

        if (script.empty()){

          // script is not cached, we have to form it, for doing so, we put all the
          // scripts of the plug-ins listening to the fired event in a single script
          // passign all thee plug-ins ids to the Run function.

          const std::string& event_plugins_ids_str = cache()->Read(event_value_hash,entity_keys::plugin_event_ids);

          if (!event_plugins_ids_str.empty()){

            std::vector<std::string> plugin_ids;
            granada::util::string::split(event_plugins_ids_str,',',plugin_ids);

            // synchronously run plug-ins
            response_data = Run(plugin_ids,event_name,parameters);

          }

        }else{
          response_data = Run(script,event_name,parameters);
        }

        response[entity_keys::plugin_parameter_data] = response_data;
        success(response);

      }
    }


    web::json::value SpidermonkeyPluginHandler::SendMessage(const std::string& from, const std::vector<std::string>& to_ids, const web::json::value& message){

      std::string script = MultiplePluginScript(to_ids);

      web::json::value response_data;
      
      if (!script.empty()){

        script =  script + " __multiOnMessage(" + granada::util::string::stringified_json(message.serialize()) + ",\"" + from + "\");";

        // wait until runner is usable, it is recommended to 
        // limit the use of the runner so it does not harm
        // other users performance.
        RunnerLock();

        // run script and parse response to JSON.
        response_data = granada::util::string::to_json(runner()->Run(script));

      }

      return response_data;
    }


    bool SpidermonkeyPluginHandler::Load(const std::shared_ptr<granada::plugin::Plugin>& plugin, const web::json::value& loader){

      const std::string& plugin_loader_hash = plugin_loader_value_hash(plugin->GetId());
      const std::string& header_str = cache()->Read(plugin_loader_hash,entity_keys::plugin_header);

      if (!header_str.empty()){

        // set the plugin header.
        plugin->SetHeader(granada::util::string::to_json(header_str));

        // retrieve and set the plug-in configuration
        const std::string& configuration_path = cache()->Read(plugin_loader_hash,entity_keys::plugin_configuration);
        web::json::value configuration = granada::util::file::ContentAsJSON(configuration_path);
        
        // Fire an event before plug-in configuration is loaded.
        // this event can be used to transform the plug-in configuration.
        web::json::value parameters = web::json::value::object();
        parameters[entity_keys::plugin_id] = web::json::value::string(plugin->GetId());
        parameters[entity_keys::plugin_configuration] = configuration;
        
        const std::string& configuration_load_before_event = default_strings::plugin_configuration_load_event + "-" + default_strings::plugin_before;
        PluginHandler::Fire(configuration_load_before_event,parameters,[&configuration](const web::json::value& data){
          const web::json::value& new_configuration = granada::util::json::first(granada::util::json::as_object(data,entity_keys::plugin_parameter_data));
          if (!new_configuration.is_null()){
            configuration = granada::util::json::as_object(new_configuration,entity_keys::plugin_parameter_data);
          }
        },[&configuration](const web::json::value& data){});

        PluginHandler::Fire(plugin->GetId() + "-" + configuration_load_before_event,parameters,[&configuration](const web::json::value& data){
          const web::json::value& new_configuration = granada::util::json::first(granada::util::json::as_object(data,entity_keys::plugin_parameter_data));
          if (!new_configuration.is_null()){
            configuration = granada::util::json::as_object(new_configuration,entity_keys::plugin_parameter_data);
          }
        },[&configuration](const web::json::value& data){});

        // set the plug-in configuration.
        plugin->SetConfiguration(configuration);

        // fire a plug-in configuration load after event.
        parameters[entity_keys::plugin_configuration] = configuration;
        const std::string& configuration_load_after_event = default_strings::plugin_configuration_load_event + "-" + default_strings::plugin_after;
        PluginHandler::Fire(configuration_load_after_event,parameters);
        PluginHandler::Fire(plugin->GetId() + "-" + configuration_load_after_event,parameters);
        
        // retrieve and set the plug-in script.
        const std::string& script_path = cache()->Read(plugin_loader_hash,entity_keys::plugin_script);
        plugin->SetScript(granada::util::file::ContentAsString(script_path));

        // plug-in successfully loaded.
        return true;
      }

      // error loading plug-in.
      return false;

    }


    std::string SpidermonkeyPluginHandler::MultiplePluginScript(const std::vector<std::string>& plugin_ids){
      // synchronously run plug-ins
      int i = 0;
      
      std::string script = "";
      std::string configurations = "";
      std::shared_ptr<granada::plugin::Plugin> plugin;
      for (auto it = plugin_ids.begin(); it != plugin_ids.end(); ++it){

        plugin = GetPluginById(*it);

        if (plugin.get() != nullptr){

          if (i > 0){
            script += ",\"" + *it + "\":" + plugin->GetScript();
            configurations += ",\"" + *it + "\":" + plugin->GetConfiguration().serialize();
          }else{
            script += "\"" + *it + "\":" + plugin->GetScript();
            configurations += "\"" + *it + "\":" + plugin->GetConfiguration().serialize();
            i++;
          }

        }
      }

      if (!script.empty()){

        script = " var __PLUGINS = {" + script + "}; ";
        configurations = " var __PLUGINS_CONFIGURATIONS = {" + configurations + "}; ";
        
        script = "var __PLUGIN; " + GetJavaScriptPluginCore(plugin) + script + configurations;

      }
      
      return script;

    }


    web::json::value SpidermonkeyPluginHandler::Run(std::string& script, const std::string& event_name, web::json::value& parameters){
      
      if (script.empty()){

        // inform the script is empty.
        web::json::value response = web::json::value::object();
        response[default_strings::plugin_error] = web::json::value::string(default_errors::plugin_empty_script);
        return response;
      }else{
        const std::string& parameters_str = granada::util::string::stringified_json(parameters.serialize());

        if (event_name.empty()){
          script =  script + " __runPlugins(" + parameters_str + ",null);";
        }else{
          script =  script + " __runPlugins(" + parameters_str + ",\"" + event_name + "\");";
        }

        // wait until runner is usable, it is recommended to 
        // limit the use of the runner so it does not harm
        // other users performance.
        RunnerLock();

        // run the script and parse response to JSON.
        return granada::util::string::to_json(runner()->Run(script));
      }
    }



    std::string SpidermonkeyPluginHandler::GetJavaScriptPluginCore(const std::shared_ptr<granada::plugin::Plugin>& plugin){
      std::string script_extension = javascript_plugin_core_;
      std::unordered_map<std::string,std::string> values;
      values.insert(std::make_pair(entity_keys::plugin_parameter_plugin_id,plugin->GetId()));
      values.insert(std::make_pair(entity_keys::plugin_parameter_plugin_handler_id,id_));
      values.insert(std::make_pair(entity_keys::plugin_parameter_configuration,plugin->GetConfiguration().serialize()));
      granada::util::string::replace(script_extension,values);
      return script_extension;
    }


    std::string SpidermonkeyPluginHandler::GetJavaScriptPluginExtension(const std::shared_ptr<granada::plugin::Plugin>& plugin){
      std::string script_inheritance = javascript_plugin_extension_;
      std::unordered_map<std::string,std::string> values;
      values.insert(std::make_pair(entity_keys::plugin_parameter_configuration,plugin->GetConfiguration().serialize()));
      granada::util::string::replace(script_inheritance,values);
      return script_inheritance;
    }



  }
}
