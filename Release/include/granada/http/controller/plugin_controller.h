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
  * Plugin controller, execute user plugins and give a response to the client.
  * The communication between the client and the plugin and vice versa is done
  * using JSON.
  *
  */

#pragma once
#include <deque>
#include <vector>
#include "cpprest/details/basic_types.h"
#include "cpprest/json.h"
#include "cpprest/details/http_helpers.h"
#include "granada/defaults.h"
#include "granada/util/mutex.h"
#include "granada/util/time.h"
#include "granada/util/string.h"
#include "granada/http/session/session.h"
#include "granada/plugin/plugin.h"
#include "granada/http/controller/controller.h"

namespace granada{

  namespace http{

    namespace controller{

      /**
       * Plugin controller, execute user plugins and give a response to the client.
       * The communication between the client and the plugin and vice versa is done
       * using JSON.
       */
      class PluginController : public Controller
      {
        public:

          /**
           * Constructor.
           * 
           * @param   url                 URI the controller listens to.
           * @param   session_factory  Session factory. Allows to have a unique point for
           *                              checking and setting sessions. Used to create a new
           *                              session if it does not exist or if it is timed out.
           */
          PluginController(utility::string_t url,std::shared_ptr<granada::http::session::SessionFactory> session_factory, std::shared_ptr<granada::plugin::PluginFactory> plugin_factory);


          /**
           * Destructor
           */
          virtual ~PluginController(){};
          

        protected:
          
          /**
           * The minimum time in milliseconds a user has to
           * wait between each plug-in handler use.
           * This property takes the value of the "plugin_handler_use_frequency_limit"
           * property from the server.conf file. If the property is not set
           * in the server configuration file, there will be no use limit.
           */
          static int PLUGIN_HANDLER_USE_FREQUENCY_LIMIT_;


          /**
           * True if the client is allowed to fire plug-in events
           * in the server. False if not. True by default.
           * The value of this property is taken from the property
           * "plugin_allow_client_to_fire_events" of the server
           * configuration file, if the property is not in the server.conf
           * file TRUE will be the default value.
           */
          static bool ALLOW_CLIENT_TO_FIRE_EVENTS_;


          /**
           * True if the client is allowed to run plug-ins
           * in the server. False if not. True by default.
           * The value of this property is taken from the property
           * "plugin_allow_client_to_run_plugins" of the server
           * configuration file, if the property is not in the server.conf
           * file TRUE will be the default value.
           */
          static bool ALLOW_CLIENT_TO_RUN_PLUGINS_;


          /**
           * True if the client is allowed to send messages
           * to the plug-ins in the server. False if not.
           * True by default.
           * The value of this property is taken from the property
           * "plugin_allow_client_to_send_messages" of the server
           * configuration file, if the property is not in the server.conf
           * file TRUE will be the default value.
           */
          static bool ALLOW_CLIENT_TO_SEND_MESSAGES_;
          

          /**
           * True if the client is allowed to run commands
           * such as reset the plug-in handler or stop the
           * plug-in handler. False if not. True by default.
           * The value of this property is taken from the property
           * "plugin_allow_client_to_run_commands" of the server
           * configuration file, if the property is not in the server.conf
           * file TRUE will be the default value.
           */
          static bool ALLOW_CLIENT_TO_RUN_COMMANDS_;


          /**
           * Where the public plug-ins repositories are. the value is taken from the
           * "plugin_publicfiles_directory" property in the server.conf file.
           * If there is no value assigned, a default value will be taken from
           * defaults.h.
           */
          static std::string PUBLICFILES_DIRECTORY_;


          /**
           * Where the user plug-ins repositories are. the value is taken from the
           * "plugin_publicfiles_directory" property in the server.conf file.
           * If there is no value assigned, a default value will be taken from
           * defaults.h.
           */
          static std::string USERFILES_DIRECTORY_;


          /**
           * Used for loading the properties only once.
           */
          static granada::util::mutex::call_once load_properties_call_once_;


          /**
           * Loads the controller properties from the server configuration file,
           * if properties and their values are not indicated in the server.conf
           * file then take the values from granada/defaults.h.                   
           */
          virtual void LoadProperties();


          /**
           * Function for loading true/false properties.
           * 
           * @param property_name Name of the property in the server.conf file.
           * @param property      Property.
           */
          virtual void LoadClientPermissionProperty(const std::string& property_name,bool& property);


          /**
           * Initializes a Plug-in Handler if it is not already initialized. It associates
           * a Plug-in Handler with a session. The id of the Plug-in Handler will be equal to
           * the session token.
           * 
           * @param session Session.
           */
          virtual void PluginHandlerInitialization(granada::plugin::PluginHandler* plugin_handler, granada::http::session::Session* session);


          /**
           * Locks the thread until Plug-in Handler is usable. Plug-in Handler use
           * can be limited using the PLUGIN_HANDLER_USE_FREQUENCY_LIMIT_ to prevent
           * abuse.
           * 
           * @param plugin_handler Plug-in Handler.
           */
          virtual void PluginHandlerLock(granada::plugin::PluginHandler* plugin_handler);


          /**
           * Fires Plug-in event
           * 
           * @param request_json    Request containing the parameters to pass to the plug-ins when firing event.
           *                        Example:@code
           *                            {"event":"calculate","parameters":{"number":"5416","addend1":"5416","addend2":"25","factor1":"5416","factor2":"2"}}@endcode
           * @param plugin_handler  Plugin Handler.
           * 
           * @return                Response to the event.
           *                        Example:@code
           *                            {
           *                              "data": {
           *                                "math.multiplication": {
           *                                  "data": {
           *                                    "message": {
           *                                      "data": {
           *                                        "math.square": {
           *                                          "data": {
           *                                            "response": "Hello I'm the square plug-in!!!"
           *                                          }
           *                                        },
           *                                        "math.sum": {
           *                                          "data": {
           *                                            "response": "Hello I'm the sum plug-in!!!"
           *                                          }
           *                                        }
           *                                      }
           *                                    },
           *                                    "old_value": "10832",
           *                                    "result": 10832
           *                                  }
           *                                }
           *                              }
           *                            }@endcode
           */
          virtual web::json::value FireEvent(const web::json::value& request_json, granada::plugin::PluginHandler* plugin_handler);


          /**
           * Runs a plug-in with the given event
           * 
           * @param request_json    Request containing the parameters to pass to the plug-ins when firing event.
           *                        Example:@code
           *                            {"plugin_id":"math.sum","parameters":{"addend1":"8","addend2":"5"}@endcode
           * @param plugin_handler  Plugin Handler.
           * 
           * @return                Plug-in response.
           *                        Example:@code
           *                            {
           *                              "data": {
           *                                "math.sum": {
           *                                  "data": {
           *                                    "configurationtest": {
           *                                      "mainContainerId": {
           *                                        "editor": "text",
           *                                        "value": "demo-container"
           *                                      }
           *                                    },
           *                                    "message": {
           *                                      "data": {
           *                                        "math.multiplication": {
           *                                          "data": {
           *                                            "response": "Hello I'm the multiplication plug-in!!!"
           *                                          }
           *                                        },
           *                                        "math.square": {
           *                                          "data": {
           *                                            "response": "Hello I'm the square plug-in!!!"
           *                                          }
           *                                        }
           *                                      }
           *                                    },
           *                                    "old_value": "5441",
           *                                    "result": 5441
           *                                  }
           *                                }
           *                              }
           *                            }@endcode
           */
          virtual web::json::value RunPlugin(const web::json::value& request_json, granada::plugin::PluginHandler* plugin_handler);


          /**
           * Send message to plug-ins.
           * 
           * @param request_json    Request containing the parameters to pass to the plug-ins when firing event.
           *                        Example:@code
           *                            {"from":"client","to_ids":["math.sum"],"parameters":{"param1":"hi"}}@endcode
           * @param plugin_handler  Plugin Handler.
           * 
           * @return                Response.
           *                        Example:@code
           *                            {"data":{"math.sum":{"data":{"response":"Hello I'm the sum plug-in!!!"}}}}@endcode
           */
          virtual web::json::value SendMessage(const web::json::value& request_json, granada::plugin::PluginHandler* plugin_handler);
          

          /**
           * Runs commands such as "reset" or "stop" to Reset or Stop the Plug-in Handler.
           * 
           * @param request_json    Request containing the parameters to pass to the plug-ins when firing event.
           *                        Example:@code
           *                            {"command":"reset"}@endcode
           * @param plugin_handler  Plugin Handler.
           * 
           * @return                Command Response.
           */
          virtual web::json::value RunCommand(const web::json::value& request_json, granada::plugin::PluginHandler* plugin_handler);


          /**
           * Handles POST HTTP request.
           * 
           * @param request HTTP request.
           */
          virtual void handle_post(web::http::http_request request);


          /**
           * Object for creating a session if it does not exist or if it is timed out.
           */
          std::shared_ptr<granada::http::session::SessionFactory> session_factory_;


          /**
           * Object for creating PluginHandlers and Plugins.
           */
          std::shared_ptr<granada::plugin::PluginFactory> plugin_factory_;

      };
    }
  }
}
