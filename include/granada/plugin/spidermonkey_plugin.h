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
  * Spidermonkey Plug-in: Defined structure to extend an application using Mozilla
  * JavaScript engine "Spidermonkey" to run the plug-ins scripts.
  *
  */

#pragma once
#include "granada/plugin/plugin.h"
#include "granada/cache/cache_handler.h"
#include "granada/runner/spidermonkey_javascript_runner.h"


namespace granada{

  namespace plugin{


    /**
     * Plugin Handler: Handles the lifecycle and communication of server side plugins.
     * Manages plug-ins lifecycle: Loads and adds plug-ins, runs plug-ins and removes them.
     * This is an abstract class that can't work on its own. Cache and Plug-in Factory
     * need to be overridden.
     * 
     * Use example:@code
     * // creating a plug-in that multiplies a number by a pre-configured factor.
     * // create header
     * // {"id":"product.tax","events":["calculate-tax","save-product-before"],"extends":["math.multiplication"]}
     * web::json::value header = web::json::value::parse("{\"id\":\"product.tax\",\"events\":[\"calculate-tax\",\"save-product-before\"],\"extends\":[\"math.multiplication\"]}");
     * 
     * // create configuration
     * // {"USTaxFactor":{"value":0.07,"editor":"number-2-decimal"},"FRTaxFactor":{"value":0.18,"editor":"number-2-decimal"}}
     * web::json::value header = web::json::value::parse("{\"USTaxFactor\":{\"value\":0.07,\"editor\":\"number-2-decimal\"},\"FRTaxFactor\":{\"value\":0.20,\"editor\":\"number-2-decimal\"}}");
     * 
     * // script -----------------------
     * std::string script = "{
     *  run : function(parameters){
     *    var me = this;
     *    var result = -1;
     *    var configuration = getConfiguration();
     *    // default tax factor
     *    var taxFactor = null;
     *    if (parameters["currency"] && configuration["taxFactor"] && configuration["USTaxFactor"]["value"]){
     *      if (parameters["currency"] == "EUR"){
     *        taxFactor = configuration["FRTaxFactor"]["value"];
     *      }else{
     *        taxFactor = configuration["USTaxFactor"]["value"];
     *      }
     *    }else{
     *      taxFactor = 0.07;
     *    }
     *    if (parameters["price"] && taxFactor){
     *      parameters["price"] = me.multiplication(parameters["price"],taxFactor);
     *    }
     *    return parameters;
     *  }
     * }";
     * // end script --------------------------
     * 
     * // Adding the plug-in
     * plugin_handler->Add(header,configuration,script);
     * 
     * // Running the plug-in, 2 options
     * // 1) Firing an event:
     * web::json::value parameters = web::json::value::parse("{\"price\":24,\"currency\":\"USD\"}");
     * 
     * plugin_handler->Fire("save-product-before",parameters,[=](const web::json::value& data){
     *  // success callback.
     * },[=](const web::json::value& data){
     *  // error callback
     * });
     * 
     * 
     * // 2) Retrieving the plug-in by its id:
     * std::shared_ptr<granada::plugin::Plugin> plugin = plugin_handler->GetPluginById("product.tax");
     * 
     * web::json::value parameters = web::json::value::parse("{\"price\":24,\"currency\":\"USD\"}");
     * 
     * plugin->Run(parameters,[=](const web::json::value& data){
     * // success callback.
     * },[=](const web::json::value& data){
     *  // error callback
     * });
     * @endcode
     */
    class SpidermonkeyPluginHandler : public PluginHandler{

      public:

        /**
         * Constructor.
         * Plug-in handler without an id.
         * Load Plug-in Handler properties.
         */
        SpidermonkeyPluginHandler(){};


        /**
         * Constructor.
         * Assigns an id to the plug-in handler.
         * Load Plug-in Handler properties.
         * 
         * @param   id  Unique Identifier of the PluginHandler.
         */
        SpidermonkeyPluginHandler(const std::string id){};


        /**
         * Destructor.
         */
        virtual ~SpidermonkeyPluginHandler(){};


        /**
         * Loads a plug-in, and returns true if plug-in loaded successfully,
         * false if not. This function is normally called by the Plug-in
         * Handler when the event loader of the plug-in is fired. Can be also
         * called to force the plug-in load
         * 
         * @param plugin    Pointer to the plug-in to load.
         * @param loader    Plug-in loader. JSON object containing the information
         *                  about how to load a plug-in.
         *                  Example:@code
         *                      {
         *                          "events"    :   ["init-ph-after"]
         *                      }@endcode
         *
         * @return             True if plug-in loaded successfully, false if not.
         */
        virtual bool Load(const std::shared_ptr<granada::plugin::Plugin>& plugin, const web::json::value& loader);


        /**
         * Extends other plug-ins indicated in the "extends" field
         * contained in the plug-in header.
         * Example of header extends field:@code
         *                    "extends":["math.calculus"]@endcode
         * If this method is called, the plug-in will extend "math.calculus"
         * plug-in, this means that plug-in will integrate the extended plug-in
         * members that it does not override.
         * Both plug-in "extends" members are concatenated.
         * 
         * @param plugin    Pointer to the plug-in that is going to extend
         *                  other plug-ins.
         */
        virtual void Extend(const std::shared_ptr<granada::plugin::Plugin>& plugin);


        /**
         * Extends only one plug-in.
         * If this method is called, the "plugin" will extend "extended_plugin",
         * this means that "plugin" will integrate the "extended_plugin"
         * members that it does not override.
         * Both plug-in "extends" members are concatenated.
         * 
         * @param extended_plugin   Pointer to the plug-in that is going to be
         *                          extended.
         * @param plugin            Pointer to the plug-in that is going to extend
         *                          other plug-ins.
         */
        virtual void Extend(const std::shared_ptr<granada::plugin::Plugin>& extended_plugin, const std::shared_ptr<granada::plugin::Plugin>& plugin);


        /**
         * Cross Extends plug-ins.
         * If this method is called, the "plugin" will extend "extended_plugins",
         * this means that "plugin" will integrate the "extended_plugins"
         * members that it does not override.
         * Both plug-in "extends" members are concatenated.
         * 
         * @param extended_plugins  Vector of Pointers to the plug-ins that are going to be
         *                          extended.
         * @param plugin            Pointer to the plug-in that is going to extend
         *                          other plug-ins.
         */
        virtual void Extend(const std::vector<std::shared_ptr<granada::plugin::Plugin>>& extended_plugins, const std::shared_ptr<granada::plugin::Plugin>& plugin);


        /**
         * Runs a plug-in. Call a success or failure callback, depending if the plug-in was
         * successfully run or not.
         * 
         * @param plugin      Pointer to the plug-in that has to be run.
         * @param parameters  JSON with the parameters to pass to the plug-in.
         *                    Example: {"addend1":"4","addend2":"6"}
         * @param event_name  Name of the event that has triggered the plug-in,
         *                    empty if the plug-in was not triggered firing an event.
         * @param success     Callback function called after the plug-in has been run with a response
         *                    in form of JSON.
         *                    Example:@code
         *                              {
         *                                "data" :
         *                                          {
         *                                            "math.sum" : {"data":{"response":10}},
         *                                          }
         *                              }@endcode
         * @param failure     Callback function called if the plug-in is not run successfully with a response
         *                    in form of a JSON, in such case normally containing an error code and
         *                    an error description.
         *                    Example:@code
         *                            {
         *                              "error" : "script_error"
         *                            }@endcode
         */
        virtual void Run(const std::shared_ptr<granada::plugin::Plugin>& plugin, web::json::value& parameters,const std::string& event_name, function_void_json success, function_void_json failure);


        /**
         * Runs multiple plug-ins at a time. Used by the Fire function to run the plug-ins listening
         * to a certain event.
         * 
         * @param plugin_ids  Vector of strings containing the ids of the plug-ins to run.
         * @param event_name  Name of the event, or empty string if there is no event, to pass to the plug-ins.
         * @param parameters  JSON with the Params to pass to the plug-ins.
         *                    Example: {"number":"9285","factor1":"9285","factor2":"4",addend1":"9285","addend2":"25"}
         * @return            JSON object with the response of the plug-in.
         *                    Example:@code
         *                              {
         *                                "data" :
         *                                          {
         *                                            "math.sum" : {"data":{"response":9310}},
         *                                            "math.multiplication" : {"data":{"response":37140}}
         *                                          }
         *                              }
         *                    If there was an error in one of the plug-ins:
         *                              {
         *                                "data" :
         *                                          {
         *                                            "math.sum" : {"data":{"response":9310}},
         *                                            "math.multiplication" : {"error":"undefined_plugin", "error_description":"Plug-in could not be found with given plug-in id."}
         *                                          }
         *                              }@endcode
         */
        virtual web::json::value Run(const std::vector<std::string>& plugin_ids, const std::string& event_name, web::json::value& parameters);


        /**
         * Called to fire an event. All plug-ins listening to that event
         * will run.
         * 
         * @param event_name  Name of the event.
         * @param parameters  Parameters in form of JSON object that are passed to the plug-ins.
         *                    that listen to the event. Example: {"number":"9285","factor1":"9285","factor2":"4",addend1":"9285","addend2":"25"}
         * @param success     Callback function called when the plug-ins successfully run passing the plug-ins responses
         *                    in form of a JSON object:@code
         *                              {
         *                                "data" :
         *                                          {
         *                                            "math.sum" : {"data":{"response":9310}},
         *                                            "math.multiplication" : {"data":{"response":37140}}
         *                                          }
         *                              }@endcode
         *                    If there was an error in one of the plug-ins:@code
         *                              {
         *                                "data" :
         *                                          {
         *                                            "math.sum" : {"data":{"response":9310}},
         *                                            "math.multiplication" : {"error":"undefined_plugin", "error_description":"Plug-in could not be found with given plug-in id."}
         *                                          }
         *                              }@endcode
         * @param failure     Callback function called when there is a problem running plug-ins with the error code and error description.
         */
        virtual void Fire(const std::string& event_name, web::json::value& parameters, function_void_json success, function_void_json failure);


        /**
         * Send messag to more than one plug-in, and returns the responses to the message in form of JSON value.
         * 
         * @param from          Id of the sender of the message. Normally a plug-in Id.
         * @param to_ids        Ids of the Plug-ins that recieve the message. Empty JSON array if we want to send the
         *                      message to all plug-ins (be-careful, doing this can seriously harm performance).
         * @param message       Message in JSON format.
         *                      Example: {"message":"square calculated! Somebody wants to tell me something?"}
         * @return              JSON object with the response to the message or JSON with an error code and an error_description
         *                      if the message couldn't be delivered correctly.
         *                      Response Examples:@code
         *                          {"data":{"math.multiplication":{"data":{"response":"8795"}}}}
         *                                    
         *                          {"math.multiplication":
         *                                  {
         *                                      "error"             : "undefined_plugin",
         *                                      "error_description" : "Plug-in could not be found with given plug-in id."
         *                                   }
         *                          }@endcode
         */
        virtual web::json::value SendMessage(const std::string& from, const std::vector<std::string>& to_ids, const web::json::value& message);


      protected:

        /**
         * Javascript basic functions for parsing communications
         * between javascript plug-ins and C++ functions.
         */
        const std::string javascript_plugin_core_ =
          #include "javascript-plugin-core.min.js"
        ;


        /**
         * Javascript functions for extending plug-ins.
         */
        const std::string javascript_plugin_extension_ =
          #include "javascript-plugin-extension.min.js"
        ;


        /**
         * Joins and returns multiple plug-in scripts and configurations
         * in one script.
         * 
         * @param plugin_ids    Vector with the ids of the plug-in.
         * @return              Joined plug-in scripts and configurations.
         *                      Example:@code
         *                      
         *                           var __PLUGIN;
         *                           [javascript-plugin-core.js]
         *                           var __PLUGINS = {"math.sum":{...},"math.square":{...}};
         *                           var __PLUGINS_CONFIGURATIONS = {{"math.sum":{...},"math.square":{...}};
         *                           
         *                      
         *                      @endcode
         */
        virtual std::string MultiplePluginScript(const std::vector<std::string>& plugin_ids);


        /**
         * Run multiple plug-ins at a time using a script containing more than one plug-in.
         * Used by the Fire function to run the cached script of plug-ins listening to a
         * certain event.
         * 
         * @param script      Script containing more than one plug-in.
         * @param event_name  Name of the event, or empty string if there is no event, to pass to the plug-ins.
         * @param parameters      JSON with the Params to pass to the plug-ins.
         * @return            JSON object with the response of the plug-in.
         */
        virtual web::json::value Run(std::string& script, const std::string& event_name, web::json::value& parameters);


        /**
         * Returns the Javascript basic functions for parsing communications
         * between javascript plug-ins and C++ functions. Inserts the values of
         * the plug-in in the script such as the Plug-in id, the Plug-in Handler
         * id, the plug-in configuration.
         * 
         * @param plugin    Plugin
         * @return          Javascript plugin basic functions.
         */
        virtual std::string GetJavaScriptPluginCore(const std::shared_ptr<granada::plugin::Plugin>& plugin);


        /**
         * Returns the Javascript functions for applying script inheritance.
         * Inserts the plug-in configuration in the script.
         * 
         * @param plugin    Plugin
         * @return          Javascript plugin basic functions.
         */
        virtual std::string GetJavaScriptPluginExtension(const std::shared_ptr<granada::plugin::Plugin>& plugin);

    };


    /**
     * Extension of the server application. Plug-ins can also extend other plug-ins.
     * Not all of them may be executed, some can just have useful functions or wait
     * to be extended by others. They can communicate. 
     */
    class SpidermonkeyPlugin : public Plugin{

      public:
        
        /**
         * Constructor, load plug-in properties.
         */
        SpidermonkeyPlugin(){};
        

        /**
         * Constructor, set Plug-in Handler id and plug-in id
         * and load properties.
         * 
         * @param plugin_handler      Plug-in Handler.
         * @param id                  Id of the Plug-in. 
         */
        SpidermonkeyPlugin(granada::plugin::PluginHandler* plugin_handler,const std::string id){};


        /**
         * Constructor, set Plug-in header, Plug-in configuration,
         * Plug-in Handler id and plug-in id and load properties.
         * 
         * @param header              Plug-in header. JSON object with the plug-in header,
         *                            header contains information such as the plug-in id,
         *                            the events the plug-in must listen to, the plug-ins
         *                            the plug-in has to extend, if the plug-in is active
         *                            or not and the way the plug-in has to be loaded.
         *                            
         *                            Example of header:
         *                            
         *                            {
         *                            
         *                               "id"       : "math.sum",
         *                               
         *                               "events"   : ["calculate"],
         *                               
         *                               "extends"  :   ["math.calculus"],
         *                               
         *                               "active"   : true,
         *                               
         *                               "loader"   : {
         *                               
         *                                              "events"  : ["init-ph-after"]
         *                                              
         *                                            }
         *                                            
         *                            }
         *
         * @param configuration       A JSON object shared with the client-side plug-ins
         *                            used to configure the plug-in.
         *                            
         *                            Example of configuration:
         *                            
         *                            {
         *                            
         *                                "mainContainerId"   :   {
         *                                
         *                                                          "value" : "demo-container",
         *                                                          
         *                                                          "editor"  :   "text"
         *                                                          
         *                                                         }
         *                                                         
         *                            }
         *                            
         * @param plugin_handler      Plug-in Handler.
         * @param script              Script or path to script/executable.
         */
        SpidermonkeyPlugin(const web::json::value header, const web::json::value configuration,granada::plugin::PluginHandler* plugin_handler, const std::string script){};

    };


    /**
     * Plug-in Factory, used to instanciate SpidermonkeyPlugins and SpidermonkeyPluginHandlers.
     */
    class SpidermonkeyPluginFactory : public PluginFactory{

      public:
        virtual std::shared_ptr<granada::plugin::Plugin>Plugin(){
          return std::shared_ptr<granada::plugin::Plugin>(new granada::plugin::SpidermonkeyPlugin());
        };

        virtual std::shared_ptr<granada::plugin::Plugin>Plugin(granada::plugin::PluginHandler* plugin_handler,const std::string& id){
          return std::shared_ptr<granada::plugin::Plugin>(new granada::plugin::SpidermonkeyPlugin(plugin_handler,id));
        };

        virtual std::shared_ptr<granada::plugin::Plugin>Plugin(const web::json::value& header, const web::json::value& configuration,granada::plugin::PluginHandler* plugin_handler, const std::string& script){
          return std::shared_ptr<granada::plugin::Plugin>(new granada::plugin::SpidermonkeyPlugin(header,configuration,plugin_handler,script));
        };

        virtual std::shared_ptr<granada::plugin::PluginHandler>PluginHandler(){
          return std::shared_ptr<granada::plugin::PluginHandler>(new granada::plugin::SpidermonkeyPluginHandler());
        };

        virtual std::shared_ptr<granada::plugin::PluginHandler>PluginHandler(const std::string& id){
          return std::shared_ptr<granada::plugin::PluginHandler>(new granada::plugin::SpidermonkeyPluginHandler(id));
        };

        virtual std::shared_ptr<granada::plugin::PluginHandler>PluginHandler(granada::plugin::PluginHandler* plugin_handler){
          return std::shared_ptr<granada::plugin::PluginHandler>(plugin_handler);
        };
    };
  }
}
