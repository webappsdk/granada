/**
  * Copyright (c) <2016> Web App SDK granada <afernandez@cookinapps.io>
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
#include "granada/http/controller/plugin_controller.h"

namespace granada{
  namespace http{
    namespace controller{

      int PluginController::PLUGIN_HANDLER_USE_FREQUENCY_LIMIT_ = 0;
      bool PluginController::ALLOW_CLIENT_TO_FIRE_EVENTS_ = true;
      bool PluginController::ALLOW_CLIENT_TO_RUN_PLUGINS_ = true;
      bool PluginController::ALLOW_CLIENT_TO_SEND_MESSAGES_ = true;
      bool PluginController::ALLOW_CLIENT_TO_RUN_COMMANDS_ = true;
      std::string PluginController::PUBLICFILES_DIRECTORY_;
      std::string PluginController::USERFILES_DIRECTORY_;

      std::once_flag PluginController::properties_flag_;

      PluginController::PluginController(utility::string_t url,std::shared_ptr<granada::http::session::SessionCheckpoint> session_checkpoint, std::shared_ptr<granada::plugin::PluginFactory> plugin_factory){
        m_listener_ = std::unique_ptr<http_listener>(new http_listener(url));
        m_listener_->support(methods::POST, std::bind(&PluginController::handle_post, this, std::placeholders::_1));
        session_checkpoint_ = std::move(session_checkpoint);
        plugin_factory_ = std::move(plugin_factory);
        std::call_once(PluginController::properties_flag_, [this](){
          this->LoadProperties();
        });
      }


      void PluginController::LoadProperties(){
        // The minimum time in milliseconds that has to be
        // between a session use a plug-in handler.
        const std::string& plugin_handler_use_frequency_limit_str = granada::util::application::GetProperty(entity_keys::plugin_handler_use_frequency_limit);
        if (plugin_handler_use_frequency_limit_str.empty()){
          PluginController::PLUGIN_HANDLER_USE_FREQUENCY_LIMIT_ = default_numbers::plugin_handler_use_frequency_limit;
        }else{
          try{
            PluginController::PLUGIN_HANDLER_USE_FREQUENCY_LIMIT_ = std::stoi(plugin_handler_use_frequency_limit_str);
          }catch(const std::logic_error& e){
            PluginController::PLUGIN_HANDLER_USE_FREQUENCY_LIMIT_ = default_numbers::plugin_handler_use_frequency_limit;
          }
        }

        LoadClientPermissionProperty(entity_keys::plugin_allow_client_to_fire_events,PluginController::ALLOW_CLIENT_TO_FIRE_EVENTS_);
        LoadClientPermissionProperty(entity_keys::plugin_allow_client_to_run_plugins,PluginController::ALLOW_CLIENT_TO_RUN_PLUGINS_);
        LoadClientPermissionProperty(entity_keys::plugin_allow_client_to_send_messages,PluginController::ALLOW_CLIENT_TO_SEND_MESSAGES_);
        LoadClientPermissionProperty(entity_keys::plugin_allow_client_to_run_commands,PluginController::ALLOW_CLIENT_TO_RUN_COMMANDS_);

        const std::string& plugin_publicfiles_directory = granada::util::application::GetProperty(entity_keys::plugin_publicfiles_directory);
        if (plugin_publicfiles_directory.empty()){
          PluginController::PUBLICFILES_DIRECTORY_ = default_strings::plugin_publicfiles_directory;
        }else{
          PluginController::PUBLICFILES_DIRECTORY_ = plugin_publicfiles_directory;
        }

        const std::string& plugin_userfiles_directory = granada::util::application::GetProperty(entity_keys::plugin_userfiles_directory);
        if (plugin_userfiles_directory.empty()){
          PluginController::USERFILES_DIRECTORY_ = default_strings::plugin_userfiles_directory;
        }else{
          PluginController::USERFILES_DIRECTORY_ = plugin_userfiles_directory;
        }

        // kill the plug-in handler when the session closes.
        const std::shared_ptr<granada::http::session::Session>& session = session_checkpoint_->check();
        if (!session->close_callbacks()->Has(default_strings::plugin_function_stop_plugin_handler)){
          const std::shared_ptr<granada::plugin::PluginFactory>& plugin_factory = plugin_factory_;
          session->close_callbacks()->Add(default_strings::plugin_function_stop_plugin_handler,[plugin_factory](const web::json::value& data){
            if (data.has_field(entity_keys::session_token)){
              const web::json::value& token = data.at(entity_keys::session_token);
              if (token.is_string()){
                const std::shared_ptr<granada::plugin::PluginHandler>& plugin_handler = plugin_factory->PluginHandler(token.as_string());
                plugin_handler->Stop();
              }
            }
            return web::json::value::object();
          });
        }
      }


      void PluginController::LoadClientPermissionProperty(const std::string& property_name,bool& property){
        std::string property_value = granada::util::application::GetProperty(property_name);
        if (!property_value.empty()){
          granada::util::string::to_upper(property_value);
          if (property_value==default_strings::plugin_properties_false){
            property = false;
          }
        }
      }


      void PluginController::PluginHandlerInitialization(const std::shared_ptr<granada::plugin::PluginHandler>& plugin_handler, const std::shared_ptr<granada::http::session::Session>& session){
        std::vector<std::string> paths;

        // public plug-ins can be accessed by everybody
        paths.push_back(PluginController::PUBLICFILES_DIRECTORY_);

        // check also private plug-ins, if user is logged-in and has uploaded
        // plug-ins, load them too.
        if (session->roles()->Is(entity_keys::plugin_role_select)){
          std::string user_directory = session->roles()->GetProperty(entity_keys::plugin_role_select,default_strings::plugin_user_directory);
          if (!user_directory.empty()){
            std::string userfiles_directory = PluginController::USERFILES_DIRECTORY_;
            std::deque<std::pair<std::string,std::string>> values;
            values.push_back(std::make_pair(default_strings::plugin_user_directory,user_directory));
            granada::util::string::replace(userfiles_directory,values);
            paths.push_back(userfiles_directory);
          }
        }

        plugin_handler->Init(paths);
        
      }


      void PluginController::PluginHandlerLock(const std::shared_ptr<granada::plugin::PluginHandler>& plugin_handler){
        if (PluginController::PLUGIN_HANDLER_USE_FREQUENCY_LIMIT_>0){
          std::string plugin_handler_value_hash_str = plugin_handler->plugin_handler_value_hash();
          std::string t_str = plugin_handler->cache()->Read(plugin_handler_value_hash_str,entity_keys::plugin_handler_last_use);
          bool cache_time = true;
          if (!t_str.empty()){
            try{
              int t = std::stoi(t_str);
              int milliseconds_to_limit = PluginController::PLUGIN_HANDLER_USE_FREQUENCY_LIMIT_ - granada::util::time::get_milliseconds_span(t);
              if (milliseconds_to_limit > 0){
                // not enough time has elapsed for using
                // the runner, sleep. 
                cache_time = false;
                granada::util::time::sleep_milliseconds(milliseconds_to_limit+1);
                PluginHandlerLock(plugin_handler);
              }
            }catch(const std::logic_error& e){}
          }

          if (cache_time){
            // save new value.
            int now = granada::util::time::get_milliseconds();
            plugin_handler->cache()->Write(plugin_handler_value_hash_str,entity_keys::plugin_handler_last_use,granada::util::time::stringify(now));
          }
        }
      }


      web::json::value PluginController::FireEvent(const web::json::value& request_json, const std::shared_ptr<granada::plugin::PluginHandler>& plugin_handler){
        web::json::value response_json;

        const web::json::value& plugin_event = request_json.at(entity_keys::plugin_event);
        if (plugin_event.is_string()){
          web::json::value parameters = granada::util::json::as_object(request_json,entity_keys::plugin_parameters);
          PluginHandlerLock(plugin_handler);
          plugin_handler->Fire(plugin_event.as_string(),parameters,[&response_json](const web::json::value& data){
            // success
            response_json = std::move(data);;
          },[&response_json](const web::json::value& data){
            // failure
            response_json = std::move(data);;
          });
        }else{
          response_json = web::json::value::object();
          response_json[default_strings::plugin_error] = web::json::value::string(default_errors::plugin_malformed_parameters);
        }

        return response_json;
      }


      web::json::value PluginController::RunPlugin(const web::json::value& request_json, const std::shared_ptr<granada::plugin::PluginHandler>& plugin_handler){
        web::json::value response_json;
        const web::json::value& plugin_id_json = request_json.at(entity_keys::plugin_id);
        if (plugin_id_json.is_string()){
          const std::string& plugin_id = plugin_id_json.as_string();

          web::json::value parameters = granada::util::json::as_object(request_json,entity_keys::plugin_parameters);

          PluginHandlerLock(plugin_handler);
          plugin_handler->Run(plugin_id,parameters,[&response_json,&plugin_id](const web::json::value& data){
            web::json::value response_data = web::json::value::object();
            response_data[plugin_id] = std::move(data);
            response_json[entity_keys::plugin_parameter_data] = response_data;
          },[&response_json](const web::json::value& data){
            // failure
            response_json = std::move(data);
          });
          
        }else{
          response_json = web::json::value::object();
          response_json[default_strings::plugin_error] = web::json::value::string(default_errors::plugin_malformed_parameters);
        }

        return response_json;
      }

      
      web::json::value PluginController::SendMessage(const web::json::value& request_json, const std::shared_ptr<granada::plugin::PluginHandler>& plugin_handler){
        web::json::value response_json;

        const web::json::value& from = request_json.at(entity_keys::plugin_parameter_from);
        const web::json::value& to_ids = request_json.at(entity_keys::plugin_parameter_to_ids);

        if (from.is_string() && to_ids.is_array()){
          web::json::value parameters = granada::util::json::as_object(request_json,entity_keys::plugin_parameters);
          PluginHandlerLock(plugin_handler);
          response_json = plugin_handler->SendMessage(from.as_string(),to_ids,parameters);
          
        }else{
          response_json = web::json::value::object();
          response_json[default_strings::plugin_error] = web::json::value::string(default_errors::plugin_malformed_parameters);
        }

        return response_json;
      }


      web::json::value PluginController::RunCommand(const web::json::value& request_json, const std::shared_ptr<granada::plugin::PluginHandler>& plugin_handler){
        web::json::value response_json;

        const web::json::value& command_json = request_json.at(default_strings::plugin_command);
        if (command_json.is_string()){
          std::string command = command_json.as_string();
          granada::util::string::to_upper(command);
          if (command==default_strings::plugin_command_reset){
            PluginHandlerLock(plugin_handler);
            plugin_handler->Reset();
          }else if (command==default_strings::plugin_command_stop){
            PluginHandlerLock(plugin_handler);
            plugin_handler->Stop();
          }else{
            response_json = web::json::value::object();
            response_json[default_strings::plugin_error] = web::json::value::string(default_errors::plugin_unknown_command);
          }
        }else{
          response_json = web::json::value::object();
          response_json[default_strings::plugin_error] = web::json::value::string(default_errors::plugin_malformed_parameters);
        }

        return response_json;
      }


      void PluginController::handle_post(http_request request){
        
        // communicate with the plug-ins
        web::http::http_response response;

        // response for the client.
        web::json::value response_json;

        // retrieve client session
        const std::shared_ptr<granada::http::session::Session>& session = session_checkpoint_->check(request,response);
        session->Update();

        // create a plug-in handler, linked to the session through the session token.
        const std::shared_ptr<granada::plugin::PluginHandler>& plugin_handler = plugin_factory_->PluginHandler(session->GetToken());

        // initialize Plug-in Handler only if it is not
        // already cached in the cache.
        if (!plugin_handler->Exists()){
          // initialize plug-in handler
          PluginHandlerInitialization(plugin_handler,session);
        }


        try{
          const web::json::value& request_json = request.extract_json().get();

          if(request_json.has_field(entity_keys::plugin_event)){

            ////
            // Fire event.
            ////

            if (PluginController::ALLOW_CLIENT_TO_FIRE_EVENTS_){
              response_json = FireEvent(request_json, plugin_handler);
            }else{
              response_json = web::json::value::object();
              response_json[default_strings::plugin_error] = web::json::value::string(default_errors::plugin_forbidden_command);
            }
          }else if (request_json.has_field(entity_keys::plugin_id)){

            ////
            // Run plug-in with given id.
            ////

            if (PluginController::ALLOW_CLIENT_TO_RUN_PLUGINS_){
              response_json = RunPlugin(request_json, plugin_handler);
            }else{
              response_json = web::json::value::object();
              response_json[default_strings::plugin_error] = web::json::value::string(default_errors::plugin_forbidden_command);
            }
          }else if (request_json.has_field(entity_keys::plugin_parameter_from)
                    && request_json.has_field(entity_keys::plugin_parameter_to_ids)
                    && request_json.has_field(entity_keys::plugin_parameters)){

            ////
            // Send message to plug-ins.
            ////

            if (PluginController::ALLOW_CLIENT_TO_SEND_MESSAGES_){
              response_json = SendMessage(request_json, plugin_handler);
            }else{
              response_json = web::json::value::object();
              response_json[default_strings::plugin_error] = web::json::value::string(default_errors::plugin_forbidden_command);
            }

          }else if (request_json.has_field(default_strings::plugin_command)){

            ////
            // Run command.
            ///

            if (PluginController::ALLOW_CLIENT_TO_RUN_COMMANDS_){
              response_json = RunCommand(request_json, plugin_handler);
            }else{
              response_json = web::json::value::object();
              response_json[default_strings::plugin_error] = web::json::value::string(default_errors::plugin_forbidden_command);
            }

          }else{
            response_json = web::json::value::object();
            response_json[default_strings::plugin_error] = web::json::value::string(default_errors::plugin_malformed_parameters);
          }
        }catch(const std::exception& e){
          response_json = web::json::value::object();
          response_json[default_strings::plugin_error] = web::json::value::string(default_errors::plugin_server_error);
          response_json[default_strings::plugin_error_description] = web::json::value::string(default_error_descriptions::plugin_server_error);
        }

        response.set_body(response_json);
        response.set_status_code(status_codes::OK);
        request.reply(response);

      }
    }
  }
}
