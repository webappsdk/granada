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
  * Defined structure to extend an application in an event-driven architecture.
  * PluginHandler: Plugin Handler: Handles the lifecycle and communication of server side plugins.
  * Plugin: Extension of the application.
  *
  */

#pragma once
#include <mutex>
#include <memory>
#include <vector>
#include <string>
#include <sstream>
#include "cpprest/json.h"
#include "boost/filesystem.hpp"
#include "granada/defaults.h"
#include "granada/util/string.h"
#include "granada/util/vector.h"
#include "granada/util/json.h"
#include "granada/util/file.h"
#include "granada/util/time.h"
#include "granada/util/application.h"
#include "granada/cache/cache_handler.h"
#include "granada/runner/runner.h"


namespace granada{

  /**
   * Namespace for server plug-in framework components: Plug-ins, Plug-in Handlers, Plug-in Factories.
   */
  namespace plugin{

    class Plugin;
    class PluginFactory;
    
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
    class PluginHandler
    {
      public:


        /**
         * Constructor.
         * Plug-in handler without an id.
         * Load Plug-in Handler properties.
         */
        PluginHandler(){};


        /**
         * Constructor.
         * Assigns an id to the plug-in handler.
         * Load Plug-in Handler properties.
         * 
         * @param   id  Unique Identifier of the PluginHandler.
         */
        PluginHandler(const std::string id){};


        /**
         * Destructor.
         */
        virtual ~PluginHandler(){};


        /**
         * Returns the plug-in hanler id.
         * @return Plug-in Handler id.
         */
        virtual const std::string& GetId(){
            return id_;
        };


        /**
         * Returns true if the PluginHandler exists, stored in the cache.
         * Used to know if the Plug-in Handler has been initialized or not.
         * 
         * @return  True if the PluginHandler is stored in the cache.
         *          False if it is not stored in the cache.
         */
        virtual bool Exists();


        /**
         * Initializes the Plug-in Handler, loads all plug-ins so they are ready to be run.
         * 
         * @param   paths   Paths of the repositories where the packaged plug-ins are.
         * @return          JSON object with the response to the "ph-init-after" event firing,
         *                  may contain an error, for example in case of exceeding byte limit.
         *                  Examples:@code
         *                        
         *                        {}
         *                        => everyting went OK.
         *                        
         *                        {
         *                          "error"             :   "bytes_limit_exceeded".
         *                          "error_description" :   "Plug-in Handler could not preload all plug-ins, because they exceed the byte limit. This limit is set for server security reasons. Contact the administrator if you need to increase the limit."
         *                        }@endcode
         *                        
         */
        virtual web::json::value Init(const std::vector<std::string>& paths);


        /**
         * Stops the Plug-in Handler removing the cached plug-ins and events.
         */
        virtual void Stop();
        

        /**
         * Resets the Plug-in Handler: Stops the Plug-in Handler
         * and Initializes it again.
         */
        virtual void Reset();


        /**
         * Caches the plug-in loading information so it is faster
         * to load it when needed.
         * Caches information as the paths of the scripts/executable,
         * the path of the plug-ins configurations files, the
         * plug-in header and the plug-in loading options.
         * We can also cache the script and the configuration directly,
         * not only their paths, but it can harm performance, and we
         * might be caching more data than needed if the plug-in finally
         * is never needed. For doing so override TransformPluginScriptPath
         * and TransformPluginConfigurationPath functions
         * so they return the script and the configuration JSON, not
         * the paths of the files.
         * 
         * 
         * @param header              JSON containing the information about when the plug-in's events, id,
         *                            inheritance, loader.
         *                            Header Example:@code
         *                                {
         *                                  "id"        :   "math.multiplication",
         *                                  "events"    :   ["calculate"],
         *                                  "extends"   :   ["math.calculus"],
         *                                  "loader"    :   {"events":["calculate-square","calculate"]}
         *                                }
         *                                
         *                                => Looking at this example, plug-in will be loaded the first
         *                                time "calculate-square" or "calculate" events are fired. If
         *                                the events are never fired, the plug-in will never load.@endcode
         *                                
         * @param configuration       JSON containing the values for configuring of the plug-in.
         *                            Configuration Example:@code
         *                                {
         *                                  "mainContainerId" : {
         *                                                        "value"   : "demo-container",
         *                                                        "editor"  : "text"
         *                                                      }
         *                                }@endcode
         * @param script              Script/Executable to be run (or the path to the script/executable).
         * @return                    True if plug-in successfully preloaded, false if not.
         */
        virtual bool AddPluginLoader(web::json::value& header, const std::string& configuration, const std::string& script);


        /**
         * Removes a plug-in loader from the cache.
         * 
         * @param plugin_id   Id of the plug-in owner of the loader.
         */
        virtual void RemovePluginLoader(const std::string& plugin_id);


        /**
         * Associates a plug-in loader to one event, 
         * the plug-in will be loaded when the event is fired.
         * 
         * @param event_name    Event name
         * @param plugin_id     Id of the plug-in.
         * @param plugin_loader Plug-in loader, preferences for plug-in load.
         *                      Loader Examples:@code
         *                                {}
         *                                => plug-in will be loaded when the event is fired.
         *                                
         *                                {"load":"eager"}
         *                                => plug-in will be loaded now.
         *                                      
         *                                {"events":["calculate-square","calculate"]}
         *                                =>  plug-in will be loaded the first time "calculate-square"
         *                                    or "calculate" event is fired.@endcode
         */
        virtual void AddLoadEvent(const std::string& event_name,const std::string& plugin_id,const web::json::value& plugin_loader);


        /**
         * Associates a plug-in loader contained in the header "loader" field
         * to one or more events contained in the loader "events" field.
         * If the loader "events" field does not exists, header "events" field
         * will be taken instead, if none of them exist, plug-in will be loader
         * on "ph-init-after" (After Plug-in Handler initialization) event.
         * 
         * @param plugin_id   Id of the plug-in.
         * @param header      Plug-in header containing the loader.
         *                    Header Examples:@code
         *                                      {}
         *                                      => plug-in will be loaded when the event "ph-init-after" is fired.
         *                                     
         *                                      {"loader":{"load":"eager"}}
         *                                      => plug-in will be loaded now.
         *                                      
         *                                      {"loader":{"events":["calculate-square","calculate"]}}
         *                                      =>  plug-in will be loaded the first time "calculate-square"
         *                                          or "calculate" event is fired.
         *                                          
         *                                      {"events":["calculate-square","calculate"],"loader":{}}
         *                                      =>  plug-in will be loaded the first time "calculate-square"
         *                                          or "calculate" event is fired.@endcode
         */
        virtual void AddLoadEvent(const std::string& plugin_id,const web::json::value& header);


        /**
         * Removes loader event.
         * 
         * @param event_name Name of the event.
         */
        virtual void RemoveLoadEvent(const std::string& event_name);


        /**
         * Adds a plug-in.
         * 
         * @param header              Stringified JSON containing the information about when the plug-in's events, id, inheritance.
         *                            Example:@code
         *                                {
         *                                  "id"      :       "math.multiplication",
         *                                  "events"  :       ["calculate"],
         *                                  "extends" :       ["math.calculus"]
         *                                }@endcode
         * @param configuration       Stringified JSON containing the values for configuring of the plug-in.
         *                            Example:@code
         *                                {
         *                                  "mainContainerId" : {
         *                                                        "value"   : "demo-container",
         *                                                        "editor"  : "text"
         *                                                      }
         *                                }@endcode
         * @param script              Script/Executable to be run (or the path to the script/executable).
         * @return                    True if plug-in has been added correctly, false if it has not.
         */
        virtual bool Add(const std::string& header, const std::string& configuration, const std::string& script);


        /**
         * Adds a plug-in.
         * 
         * @param header              JSON containing the information about when the plug-in's events, id, inheritance.
         *                            Example:@code
         *                                {
         *                                  "id"      :       "math.multiplication",
         *                                  "events"  :       ["calculate"],
         *                                  "extends" :       ["math.calculus"]
         *                                }@endcode
         * @param configuration       JSON containing the values for configuring of the plug-in.
         *                            Example:@code
         *                                {
         *                                  "mainContainerId" : {
         *                                                        "value"   : "demo-container",
         *                                                        "editor"  : "text"
         *                                                      }
         *                                }@endcode
         * @param script              Script/Executable to be run (or the path to the script/executable).
         * @return                    True if plug-in has been added correctly, false if it has not.
         */
        virtual bool Add(web::json::value& header, const web::json::value& configuration, const std::string& script);


        /**
         * Adds a plug-in.
         * 
         * @param plugin  Plug-in to add.
         * @return        True if plug-in has been added correctly, false if it has not.
         */
        virtual bool Add(const std::shared_ptr<granada::plugin::Plugin>& plugin);


        /**
         * Adds a plug-in with parameters to pass to the plug-in in case
         * it is executed just after adding it, this happens for example
         * if the plug-in header does not have an "events" field.
         * This is done specifying if the plug-in can be run immediately or not.
         * 
         * @param plugin        Plug-in to add.
         * @param parameters    Parameters, useful in case we add a plug-in and it is
         *                      executed just after adding it.
         * @return              True if plug-in has been added correctly, false if it has not.
         */
        virtual bool Add(const std::shared_ptr<granada::plugin::Plugin>& plugin, web::json::value& parameters);


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
        virtual void Extend(const std::vector<std::shared_ptr<granada::plugin::Plugin>>& extended_plugins, const std::shared_ptr<granada::plugin::Plugin>& plugin){
            // override
        };


        /**
         * Adds an extension to a plug-in, so when the plug-in is added it
         * is extended. Notice that after extending a plug-in, the
         * extended plug-in is no more active.
         * 
         * @param extended_plugin_id  Id of the plug-in to extend.
         * @param plugin_id           Id of the extension.
         */
        virtual void AddExtension(const std::string& extended_plugin_id,const std::string& plugin_id);


        /**
         * If a plug-in is extended by other already added
         * plug-ins extends it and returns true. Otherwise returns
         * false.
         * 
         * @return bool        True if plug-in has been extended, false if not.
         */
        virtual bool ApplyExtensions(const std::shared_ptr<granada::plugin::Plugin>& plugin);


        /**
         * Removes a plug-in extension, so when the plug-in is added, the
         * extension is no more applied. Notice that after extending a plug-in, the
         * extended plug-in is no more active.
         * 
         * @param extended_plugin_id  Id of the extended plug-in.
         * @param plugin_id           Id of the extension.
         */
        virtual void RemoveExtension(const std::string& extended_plugin_id, const std::string& plugin_id);
        

        /**
         * Removes all extensions to a plug-in, so when the plug-in is added,
         * the extensions are no more applied. Notice that when extending a plug-in, the
         * extended plug-in is no more active.
         * 
         * @param plugin_id     Id of the extended plug-in.
         */
        virtual void RemoveExtensions(const std::string& plugin_id);


        /**
         * Makes a plug-in listen to an event so it runs when the event
         * is fired.
         * 
         * @param event_name  Name of the event.
         * @param plugin_id   Id of the plug-in.
         */
        virtual void AddEventListener(const std::string& event_name, const std::string& plugin_id);


        /**
         * The plug-in with given id will no more listen to the given event.
         * 
         * @param event_name  Name of the event.
         * @param plugin_id   Id of the plug-in.
         */
        virtual void RemoveEventListener(const std::string& event_name, const std::string& plugin_id);


        /**
         * The plug-in with given id will no more listen to any event.
         * 
         * @param plugin_id   Id of the plug-in.
         */
        virtual void RemoveEventListeners(const std::string& plugin_id);


        /**
         * The plug-in pointed by the given pointer will no more listen to any event.
         * 
         * @param plugin_id   Pointer pointing to a plug-in.
         */
        virtual void RemoveEventListeners(const std::shared_ptr<granada::plugin::Plugin>& plugin);


        /**
         * Plug-in function: This function can be called from the plug-in script. It stops a plug-in from listening to
         * the events it was listening.
         * Recommended wrap function in a script: removeEvents();
         * 
         * @param parameters    JSON object with the plug-in Handler id and the id of the plug-in listening to events.
         *                      Example:@code
         *                              {
         *                                "__PLUGIN_ID"         :   "math.multiplication",
         *                                "__PLUGIN_HANDLER_ID" :   "WjdTAXePkQH7UtAWqwdy4v0qtOhFZwLn73tXHsiBn0i9BDAoH0A6WIz9ovtF3AqY"
         *                              }@endcode
         * @return              An empty JSON object {} when the plug-in has been stopped from listening to the events,
         *                      and a JSON object with an error code and an error description when the plug-in couldn't be
         *                      stopped.
         *                      Example of returned JSON when there is an error:@code
         *                             {
         *                                 "error"             : "missing_parameter",
         *                                 "error_description" : "One or more of the given parameters is missing."
         *                             }@endcode
         */
        virtual web::json::value RemoveEventListeners(const web::json::value& parameters);

        
        /**
         * Returns a pointer of a server plug-in managed by the plug-in handler with the given plug-in id.
         * 
         * @param plugin_id   Plugin id.
         * @return            Plugin pointer.
         */
        virtual std::shared_ptr<granada::plugin::Plugin> GetPluginById(const std::string& plugin_id);


        /**
         * Runs a plug-in. Call a success or failure callback, depending if the plug-in was
         * successfully run or not.
         * 
         * @param parameters  JSON with the parameters to pass to the plug-in.
         *                    Example: {"addend1":"4","addend2":"6"}
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
        virtual void Run(const std::string& plugin_id, web::json::value& parameters, function_void_json success, function_void_json failure){
            Run(plugin_id,parameters,"",success,failure);
        };


        /**
         * Runs a plug-in. Call a success or failure callback, depending if the plug-in was
         * successfully run or not.
         * 
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
        virtual void Run(const std::string& plugin_id, web::json::value& parameters, const std::string& event_name, function_void_json success, function_void_json failure);


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
        virtual web::json::value Run(const std::vector<std::string>& plugin_ids, const std::string& event_name, web::json::value& parameters){
          return web::json::value::object();
        };


        /**
         * Plug-in function: This function can be called from the plug-in script. It runs a plug-in with
         * a given id.
         * Recommended wrap in a script:@code runPlugin("math.multiplication.check.range",
         *                                         {"number":"9285","factor1":"9285","factor2":"4"},
         *                                         function(successResponseData){
         *                                            // success
         *                                         },dunction(failureResposeData){
         *                                            // failure
         *                                         });@endcode
         * 
         * @param parameters    JSON object with the plug-in Handler id, the id of the plug-in to run and the parameters
         *                      to pass to the plug-in.
         *                      Example:@code
         *                              {
         *                                "id"                  :   "math.multiplication.check.range",
         *                                "data"                :   {"number":"9285","factor1":"9285","factor2":"4"},
         *                                "__PLUGIN_ID"         :   "math.multiplication",
         *                                "__PLUGIN_HANDLER_ID" :   "WjdTAXePkQH7UtAWqwdy4v0qtOhFZwLn73tXHsiBn0i9BDAoH0A6WIz9ovtF3AqY"
         *                              }@endcode
         * @return              A JSON object with the plug-in response.
         *                      Example:@code
         *                              {
         *                                "data" :
         *                                          {
         *                                            "math.multiplication.check.range" : {"data":{"valid":true,"number":"9285","factor1":"9285","factor2":"4"}},
         *                                          }
         *                              }@endcode
         *                      Example of returned JSON when there is an error:@code
         *                             {
         *                                 "error"             : "missing_parameter",
         *                                 "error_description" : "One or more of the given parameters is missing."
         *                             }@endcode
         */
        virtual web::json::value Run(const web::json::value& parameters);


        /**
         * Remove plug-in with given id.
         * 
         * @param plugin_id   Plug-in id.
         */
        virtual void Remove(const std::string& plugin_id);


        /**
         * Plug-in function: This function can be called from the plug-in script. It removes a plug-in with
         * a given id.
         * Recommended wrap function in a script: remove("math.multiplication.check.range");
         * 
         * @param parameters    JSON object with the plug-in Handler id and the id of the plug-in to remove.
         *                      Example:@code
         *                              {
         *                                "id"                  :   "math.multiplication.check.range",
         *                                "__PLUGIN_HANDLER_ID" :   "WjdTAXePkQH7UtAWqwdy4v0qtOhFZwLn73tXHsiBn0i9BDAoH0A6WIz9ovtF3AqY"
         *                              }@endcode
         * @return              An empty JSON object {} when the plug-in has been removed, and a JSON object
         *                      with an error code and an error description when the plug-in couldn't be removed.
         *                      Example of returned JSON when there is an error:@code
         *                             {
         *                                 "error"             : "missing_parameter",
         *                                 "error_description" : "One or more of the given parameters is missing."
         *                             }@endcode
         */
        virtual web::json::value Remove(const web::json::value& parameters);



        /**
         * Called to fire an event. All plug-ins listening to that event
         * will be run.
         * 
         * @param event_name  Name of the event. Example: "entry-created-before".
         */
        virtual void Fire(const std::string& event_name);


        /**
         * Called to fire an event. All plug-ins listening to that event
         * will run.
         * 
         * @param event_name  Name of the event.
         * @param parameters  Parameters in form of JSON object that are passed to the plug-ins.
         *                    that listen to the event. Example:@code
         *                      {"number":"9285","factor1":"9285","factor2":"4",addend1":"9285","addend2":"25"}@endcode
         */
        virtual void Fire(const std::string& event_name, web::json::value& parameters);


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
         * Plug-in function: This function can be called from the plug-in script. It fires an event. All the plug-ins
         * listening to the event will be run.
         * Recommended wrap function in a script:@code fire("math.multiplication-before",
         *                                             {"number":"9285","factor1":"9285","factor2":"4"},
         *                                             function(successResponseData){
         *                                                // success
         *                                             },dunction(failureResposeData){
         *                                                // failure
         *                                             });@endcode
         * 
         * @param parameters    JSON with the Plug-in Handler id, the event to fire and the parameters to pass to the
         *                      plug-ins listening to the fired event.
         *                      Example:@code
         *                              {
         *                                "event_name"          :   "math.multiplication-before",
         *                                "data"                :   {"number":"9285","factor1":"9285","factor2":"4"},
         *                                "__PLUGIN_ID"         :   "math.multiplication",
         *                                "__PLUGIN_HANDLER_ID" :   "WjdTAXePkQH7UtAWqwdy4v0qtOhFZwLn73tXHsiBn0i9BDAoH0A6WIz9ovtF3AqY"
         *                              }@endcode
         * @return              A JSON object with the plug-ins responses.
         *                      Examples:@code
         *                              {
         *                                "data" :
         *                                          {
         *                                            "math.multiplication.check.range" : {"data":{"valid":true,"number":"9285","factor1":"9285","factor2":"4"}},
         *                                          }
         *                              }@endcode
         *                    If there was an error in one of the plug-ins:@code
         *                              {
         *                                "data" :
         *                                          {
         *                                            "math.multiplication.check.range" : {"error":"undefined_plugin", "error_description":"Plug-in could not be found with given plug-in id."}
         *                                          }
         *                              }@endcode
         */
        virtual web::json::value Fire(const web::json::value& parameters);


        /**
         * Send message to plug-ins, and returns the responses to the message in form
         * of JSON value.
         * 
         * @param from                    Id of the Entity that sends the message. Normally a plug-in Id.
         * @param to_ids                  Ids of the Plug-ins that recieve the message. Empty JSON array if we want to send the
         *                                message to all plug-ins (be-careful, doing this can seriously harm performance). 
         * @param message                 Message in JSON format.
         *                                Example: {"message":"square calculated! Somebody wants to tell me something?"}
         * @return                        JSON object with the response to the message or JSON with an error code and an error_description
         *                                if the message couldn't be delivered correctly.
         *                                Response Examples:@code
         *                                    {"data":{"math.multiplication":{"data":{"response":"8795"}}}}
         *                                    
         *                                    {"math.multiplication":
         *                                            {
         *                                              "error"             : "undefined_plugin",
         *                                              "error_description" : "Plug-in could not be found with given plug-in id."
         *                                            }
         *                                    }@endcode
         *                                    
         */
        virtual web::json::value SendMessage(const std::string& from, const web::json::value& to_ids, const web::json::value& message);


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
        virtual web::json::value SendMessage(const std::string& from, const std::vector<std::string>& to_ids, const web::json::value& message){
          return web::json::value::object();
        };


        /**
         * Plug-in function: Send message from a plug-in or other entity to a plug-in or to plug-ins.
         * Recommended wrap function in a script:@code
         *                                       sendMessage({"message":"square calculated! Somebody wants to tell me something?"},
         *                                         ["math.sum","math.multiplication"],
         *                                         function(messageResponse){
         *                                            //...
         *                                         });@endcode
         * 
         * @param parameters JSON containing the sender information an
         *                    Example:@code
         *                             {
         *                              "__PLUGIN_HANDLER_ID"     : "GkydJC7abem4pVEdzSP13kxpjS37wHqzvKtt3phLYr5n3XYhZH2CWPNYOugVvHg5",
         *                              "__PLUGIN_ID"             : "math.square",
         *                              "to_ids"                  : ["math.sum","math.multiplication"],
         *                              "data"                    : {"message":"square calculated! Somebody wants to tell me something?","result":"1759"}
         *                             }
         *                        =>  in this example message is sent to "math.sum" and "math.multiplication" plug-ins,
         *                            If "to_ids" parameter is an empty array "[]" the message will be sent to all plug-ins
         *                            except to the sender.@endcode
         * @return            Response to the message in form of JSON object with a JSON array with the message or a JSON object with an
         *                    error code and an error_description in case there was a problem.
         *                    Example:@code
         *                              {
         *                                "data" :
         *                                          {
         *                                            "math.sum" : {"data":{"response":"Do nothing"}},
         *                                            "math.multiplication" : {"data":{"response":"8795"}}
         *                                          }
         *                              } => everything is fine
         *                              
         *                              {
         *                                "error" : "missing_parameter",
         *                                "error_description" : "One or more of the given parameters is missing."
         *                              } => error sending messages@endcode
         */
        virtual web::json::value SendMessage(const web::json::value& parameters);

        
        /**
         * Plug-in function: This function can be called from the plug-in script. It caches a key-value pair
         * that can be used by the plug-in during the plug-in life.
         * Used to store plug-in global variables.
         * Recommended wrap function in script: setValue("number",8); => stores the value "8" identified by the name "number" until
         *          the plug-in is removed or the Plug-in Handler is stoped.
         * 
         * @param parameters    JSON with the plug-in id, the Plug-in Handler id and the key-value pair to store.
         *                      Example:@code
         *                              {
         *                                "key"                 :   "number",
         *                                "value"               :   "3",
         *                                "__PLUGIN_ID"         :   "math.multiplication",
         *                                "__PLUGIN_HANDLER_ID" :   "WjdTAXePkQH7UtAWqwdy4v0qtOhFZwLn73tXHsiBn0i9BDAoH0A6WIz9ovtF3AqY"
         *                              }@endcode
         * @return              An empty JSON object {} is returned if the key-value pair is stored correctly,
         *                      and a JSON with an error code and an error_description if there was a problem
         *                      when storing the key-value pair.
         *                      Example of returned JSON when there is an error:@code
         *                             {
         *                                 "error"             : "missing_parameter",
         *                                 "error_description" : "One or more of the given parameters is missing."
         *                             }@endcode
         */
        virtual web::json::value SetValue(const web::json::value& parameters);


        /**
         * Plug-in function: This function can be called from the plug-in script. It retrieves a value previously stored
         * by the plug-in.
         * Used to retrieve plug-in global variables.
         * If value does not exists returns an empty value.
         * Recommended wrap function in script: var number = getValue("number");
         * 
         * @param parameters    JSON with the plug-in id, the Plug-in Handler id and the key of the value to retrieve.
         *                      Example:@code
         *                              {
         *                                "key"                 :   "number",
         *                                "__PLUGIN_ID"         :   "math.multiplication",
         *                                "__PLUGIN_HANDLER_ID" :   "WjdTAXePkQH7UtAWqwdy4v0qtOhFZwLn73tXHsiBn0i9BDAoH0A6WIz9ovtF3AqY"
         *                              }@endcode
         * @return              A JSON object with the requested value or a JSON object with an error message, in case
         *                      the value couldn't be retrieved successfully.
         *                      Example:@code  {"value":"8"} => value successfully retrieved.
         *                                
         *                                {
         *                                    "error"             : "missing_parameter",
         *                                    "error_description" : "One or more of the given parameters is missing."
         *                                } => error retrieving value.
         *                                
         *                                {"value": ""} => value does not exists, but no error.@endcode
         */
        virtual web::json::value GetValue(const web::json::value& parameters);


        /**
         * Plug-in function: This function can be called from the plug-in script. It destroys a value previously stored
         * by the plug-in.
         * Recommended wrap function in a script: destroyValue("number");
         * 
         * @param parameters    JSON with the plug-in id, the Plug-in Handler id and the key of the value to destroy.
         *                      Example:@code
         *                              {
         *                                "key"                 :   "number",
         *                                "__PLUGIN_ID"         :   "math.multiplication",
         *                                "__PLUGIN_HANDLER_ID" :   "WjdTAXePkQH7UtAWqwdy4v0qtOhFZwLn73tXHsiBn0i9BDAoH0A6WIz9ovtF3AqY"
         *                              }@endcode
         * @return              An empty JSON object {} is returned if the key-value pair is destroyed correctly,
         *                      and a JSON object with an error code and an error_description if there was a problem
         *                      when destroying the key-value pair.
         *                      Example of returned JSON when there is an error:@code
         *                             {
         *                                 "error"             : "missing_parameter",
         *                                 "error_description" : "One or more of the given parameters is missing."
         *                             }@endcode
         */
        virtual web::json::value DestroyValue(const web::json::value& parameters);


        /**
         * Plug-in function: This function can be called from the plug-in script. It destroys all values previously stored
         * by the plug-in.
         * Recommended wrap function in a script: clearValues();
         * 
         * @param parameters    JSON with the plug-in id and the Plug-in Handler id.
         *                      Example:@code
         *                              {
         *                                "__PLUGIN_ID"         :   "math.multiplication",
         *                                "__PLUGIN_HANDLER_ID" :   "WjdTAXePkQH7UtAWqwdy4v0qtOhFZwLn73tXHsiBn0i9BDAoH0A6WIz9ovtF3AqY"
         *                              }@endcode
         * @return              An empty JSON object {} is returned if the key-value pairs are destroyed correctly,
         *                      and a JSON object with an error code and an error_description if there was a problem
         *                      when destroying the key-value pairs.
         *                      Example of returned JSON when there is an error:@code
         *                             {
         *                                 "error"             : "missing_parameter",
         *                                 "error_description" : "One or more of the given parameters is missing."
         *                             }@endcode
         */
        virtual web::json::value ClearValues(const web::json::value& parameters);


        /**
         * For increasing server performance the use of the runner is limited.
         * This function returns true if runner is usable or false if it is not,
         * normally if false is returned it is because it has been used
         * very frequently.
         * 
         * @return  True is runner is usable, false if not.
         */
        virtual void RunnerLock();


        /**
         * Returns a pointer to the Cache Handler. Used to cache plug-ins headers, loaders, configuration and
         * script paths as well as plug-ins global values. Needs to be overridden.
         * @return  Pointer to the Cache Handler.
         */
        virtual std::shared_ptr<granada::cache::CacheHandler> cache(){
          return std::shared_ptr<granada::cache::CacheHandler>(nullptr);
        };

        
        /**
         * Returns a pointer to a Plug-in Factory. Used to create PluginHandlers and Plugins.
         * @return Pointer to a plug-in Factory.
         */
        virtual std::shared_ptr<granada::plugin::PluginFactory> plugin_factory(){
          return std::shared_ptr<granada::plugin::PluginFactory>(nullptr);
        };


        /**
         * Returns a pointer to the responsible of running or executing the
         * plug-in scripts/executables.
         * @return Pointer to the scripts/executables runner.
         */
        virtual std::shared_ptr<granada::runner::Runner> runner(){
          return std::shared_ptr<granada::runner::Runner>(nullptr);
        };


        /**
         * Return the hash used to store the Plug-in Handler
         * values in the cache.
         * @return  Hash used to store the Plug-in Handler
         *          values in the cache.
         */
        virtual std::string plugin_handler_value_hash(){
          return cache_namespaces::plugin_handler_value + id_;
        };


        /**
         * Return the hash used to store the Plug-in loaders
         * in the cache.
         * @return  Hash used to store the Plug-in loaders
         *          in the cache.
         */
        virtual std::string plugin_loader_value_hash(const std::string& plugin_id){
          return cache_namespaces::plugin_loader_value + id_ + ":" + plugin_id;
        };

        
        /**
         * Return the hash used to store the Plug-in values
         * in the cache.
         * @return  Hash used to store the Plug-in values
         *          in the cache.
         */
        virtual std::string plugin_value_hash(const std::string& plugin_id){
          return cache_namespaces::plugin_value + id_ + ":" + plugin_id;
        };

        
        /**
         * Return the hash used to store the Plug-in variables
         * values in the cache.
         * @return  Hash used to store the Plug-in variables
         *          values in the cache.
         */
        virtual std::string plugin_store_hash(const std::string& plugin_handler_id, const std::string& plugin_id){
          return cache_namespaces::plugin_store + plugin_handler_id + ":" + plugin_id;
        };
        

        /**
         * Return the hash used to store the events
         * in the cache.
         * @return  Hash used to store the events
         *          in the cache.
         */
        virtual std::string plugin_event_value_hash(const std::string& event_name){
          return cache_namespaces::plugin_event_value + id_ + ":" + event_name;
        };


      protected:

        /**
         * Maximum bytes a Plug-in Handler can load.
         */
        static int PLUGIN_BYTES_LIMIT_;


        /**
         * Messages to plug-ins are not sent to each plug-in separately,
         * to increase performance, we group the plug-ins so groups
         * of them recieve the messages at the same time. 
         */
        static int SEND_MESSAGE_PLUGIN_GROUP_SIZE_;


        /**
         * The minimum time Plug-ins have to wait
         * between two runs in milliseconds.
         * Value is taken from the "plugin_runner_use_frequency_limit" property from the
         * server.conf file.
         * If the value is not indicated in the server.conf file a default value is taken
         * from defaults values, default value is: 50ms.
         */
        static int RUNNER_USE_FREQUENCY_LIMIT_;
        

        /**
         * Once flag for properties loading.
         */
        static std::once_flag properties_flag_;


        /**
         * Once flag used to set the functions that can be
         * called from the plug-ins when they are running
         * only once.
         */
        static std::once_flag functions_to_runner_flag_;
        

        /**
         * Used to give an id to plug-ins without id.
         */
        static unsigned long long uid_;


        /**
         * Mutex for multi-thread safety when increaing
         * unique ids.
         */
        static std::mutex uid_mtx_;


        /**
         * Id of the Plug-in Handler. It has to be unique.
         * It can be a unique nonce that we pass to the user. Or if we chose to link the Plug-in Handler to a session,
         * it can be the id of a session. Or the username if we link the Plug-in Handler to a user, be aware, that if 
         * a Plug-in Handler is linked to a user, the plug-ins of two users connected with the same username will be
         * shared, so they will share for example plug-ins global variables.
         */
        std::string id_;


        /**
         * Path of the repositories where the packaged plug-ins are.
         */
        std::vector<std::string> paths_;


        /**
         * Load Plug-in Handler properties.
         */
        virtual void LoadProperties();


        /**
         * Get a unique id string, used to give ids to plug-ins that don't have an id.
         * @return Unique id string
         */
        virtual std::string GetUID();


        /**
         * Preload the plug-ins of the given repositories,
         * so it is faster to load them when needed.
         * Caches information as the paths of the scripts/executable,
         * the path of the plug-ins configurations files, the
         * plug-in header and the plug-in loading options.
         * We can also cache the script and the configuration directly,
         * not only their paths, but it will be heavier to load. For doing so
         * override TransformPluginScriptPath and TransformPluginConfigurationPath functions
         * so they return the script and the configuration JSON, not
         * the paths of the files.
         * The repositories paths are in the global paths_ variable.
         */
        virtual int PreloadRepositories();


        /**
         * Preload the packaged plug-ins of one given repository,
         * so it is faster to load them when needed.
         * Caches information as the paths of the scripts/executable,
         * the path of the plug-ins configurations files, the
         * plug-in header and the plug-in loading options.
         * We can also cache the script and the configuration directly,
         * not only their paths, but it will be heavier to load. For doing so
         * override TransformPluginScriptPath and TransformPluginConfigurationPath functions
         * so they return the script and the configuration JSON, not
         * the paths of the files.
         * 
         * @param repository_path Repository path, it can be complete or relative path.
         *                        /path/to/repository   => complete path
         *                        path/to/repository    => relative path
         *                        The path will be relative to where the application is executed.
         */
        virtual int PreloadPackages(const std::string& repository_path);


        /**
         * Preload the packaged plug-ins of one given package,
         * so it is faster to load them when needed.
         * Caches information as the paths of the scripts/executable,
         * the path of the plug-ins configurations files, the
         * plug-in header and the plug-in loading options.
         * We can also cache the script and the configuration directly,
         * not only their paths, but it will be heavier to load. For doing so
         * override TransformPluginScriptPath and TransformPluginConfigurationPath functions
         * so they return the script and the configuration JSON, not
         * the paths of the files.
         * 
         * @param package_directory_path Package directory path.
         *                               path/to/repository/path/to/package
         */
        virtual int PreloadPackagedPlugins(const std::string& package_directory_path);


        /**
         * Executes all plug-in loaders listening to an event if there are, loading
         * the plug-ins.
         * 
         * @param event_name      Name of the event.
         */
        virtual void FireLoadEvent(const std::string& event_name);


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
        virtual bool Load(const std::shared_ptr<granada::plugin::Plugin>& plugin, const web::json::value& loader){
          return false;
        };


        /**
         * Transformation function, used to transform a script file path or
         * not. This function is called when preloading a plug-in, by default
         * it returns the same file path that is given,
         * but this function can be overriden so it returns the content of the file:
         * the script itself, so script is cached when preloading (and not when loading the plug-in).
         * Notice that by doing so, the preloading may be slower, and we may cache scripts
         * that are never going to be used.
         * 
         * @param plugin_script_path    Path of the script/executable.
         * @return                      Path of the script/executable, the script itself or a tranformed path.
         */
        virtual std::string TransformPluginScriptPath(const std::string& plugin_script_path);


        /**
         * Transformation function, used to transform a configuration file path or
         * not. This function is called when preloading a plug-in, by default
         * it returns the same file path that is given,
         * but this function can be overriden so it returns the content of the file:
         * the plug-in configuration JSON itself, so the configuration JSON is cached when
         * preloading (and not when loading the plug-in).
         * Notice that by doing so, the preloading may be slower, and we may cache configurations
         * that are never going to be used.
         * 
         * @param plugin_script_path    Path of the plug-in configuration.
         * @return                      Path of the plug-in configuration, configuration JSON, or a tranformed path.
         */
        virtual std::string TransformPluginConfigurationPath(const std::string& plugin_configuration_path);


        /**
         * Adds the functions that can be called from the
         * plug-ins when they are running. Call it once as
         * runner is static.
         */
        virtual void AddFunctionsToRunner();
        
    };


    /**
     * Extension of the server application. Plug-ins can also extend other plug-ins.
     * Not all of them may be executed, some can just have useful functions or wait
     * to be extended by others. They can communicate. 
     */
    class Plugin
    {
      public:


        /**
         * Constructor.
         */
        Plugin(){};


        /**
         * Constructor.
         * Loads Plug-in properties from server configuration file.
         * 
         * @param plugin_handler      Plug-in Handler.
         * @param id                  Id of the Plug-in.
         */
        Plugin(granada::plugin::PluginHandler* plugin_handler,const std::string id){};


        /**
         * Constructor.
         * Loads Plug-in properties from server configuration file.
         * 
         * @param plugin_handler      Plug-in Handler that manages the plug-in.
         * @param header              Plug-in header is a JSON object that contains information as plug-in id, events, inheritance.
         * @param configuration       JSON object containing the configuration of a plugin.
         *                            Example:@code
         *                                      {
         *                                       "factor":5,
         *                                        "url_to_check":"https://cookinapps.io"
         *                                      }@endcode
         * @param script              Script/Executable or Path of the file containing the script or the executable.
         */
        Plugin(granada::plugin::PluginHandler* plugin_handler, const web::json::value header, const web::json::value configuration, const std::string script){};


        /**
         * Destructor
         */
        virtual ~Plugin(){};


        /**
         * Returns true if the plug-in exists in the cache, false
         * if it does not.
         * 
         * @return True if plug-in is cached, false if it is not.
         */
        virtual bool Exists();
        


        /**
         * Sets the boolean value of extended,
         * true if the plug-in has been extended by others.
         * false if the plug-in has not already been extended by others.
         * Normally extended plug-ins won't run anymore, so this flag is
         * used to know if a plug-in is somekind active | inactive. 
         * @param extended    True if the plug-in has been extended by others.
         *                    False if the plug-in has not already been extended by others.
         */
        virtual void IsExtended(bool extended);


        /**
         * Returns true if the plug-in has been extended by others.
         * false if the plug-in has not already been extended by others.
         * Normally extended plug-ins won't run anymore, so this flag is
         * used to know if a plug-in is somekind active | inactive. 
         * @return    True if the plug-in has been extended by others.
         *            False if the plug-in has not already been extended by others.
         */
        virtual const bool IsExtended();


        /**
         * Returns the plug-in id.
         * @return  Plug-in id.
         */
        virtual const std::string GetId(){
          return id_;
        };


        /**
         * Sets the plug-in id.
         * @param  Plug-in id.
         */
        virtual void SetId(const std::string id){
          id_ = std::move(id);
        };


        /**
         * Returns plug-in header. Plug-in header is a JSON object containing basic information about
         * the plug-in such as plug-in id, events and what plug-in it extends.
         * Example of header:@code
         *                    {
         *                      "id" : "betterworddefinition.word-filter",
         *                      "events" : ["load-text-after","load-description-after"],
         *                      "extends" : ["worddefinition.word-filter"]
         *                    }@endcode
         * @return Plug-in header.
         */
        virtual const web::json::value& GetHeader(){
          return header_;
        };


        /**
         * Set plug-in header. Plug-in header is a JSON object containing basic information about
         * the plug-in such as plug-in id, events and what plug-in it extends.
         * 
         * @param header  JSON object containing the header parameters.
         *                Example of header:@code
         *                    {
         *                      "id" : "betterworddefinition.word-filter",
         *                      "events" : ["load-text-after","load-description-after"],
         *                      "extends" : ["worddefinition.word-filter"]
         *                    }@endcode
         */
        virtual void SetHeader(const web::json::value header){
          header_ = std::move(header);
        };


        /**
         * Returns the plug-in script/executable or plug-in script path
         * or plug-in executable path.
         * @return    Plug-in script/executable or plug-in script path
         *            or plug-in executable path.
         */ 
        virtual std::string GetScript(){
          return script_;
        };


        /**
         * Sets the plug-in script/executable or plug-in script path
         * or plug-in executable path.
         * @param     Plug-in script/executable or plug-in script path
         *            or plug-in executable path.
         */ 
        virtual void SetScript(const std::string script){
          script_ = std::move(script);
        };


        /**
         * Returns the plug-in configuration.
         * @return    JSON object with the plug-in configuration.
         */
        virtual const web::json::value& GetConfiguration(){
          return configuration_;
        };


        /**
         * Returns the plug-in configuration.
         * @param    JSON object with the plug-in configuration.
         */
        virtual const web::json::value& SetConfiguration(const web::json::value configuration){
          configuration_ = std::move(configuration);
        };


        /**
         * Returns true if plug-in is runnable, and false
         * if it is not.
         * @return  True if plug-in is runnable, and false
         * if it is not.
         */
        const bool& IsRunnable(){
            return runnable_;
        };


        /**
         * Sets the runnable status of a plug-in.
         * Set it to true if plug-in is runnable, and false
         * if it is not.
         * 
         * @param runnable  Runnable status of a plug-in.
         *                  True if plug-in is runnable, and false
         *                  if it is not.
         */
        void IsRunnable(bool runnable){
            runnable_ = std::move(runnable);
        };


      protected:

        /**
         * Once flag used for loading plug-in properties
         * only once.
         */
        static std::once_flag properties_flag_;


        /**
         * String Plug-in id unique identifier. Normally composed of
         * the name of the package + the name of the plug-in with a dot
         * as separator. Name of the plug-ins should be descriptives.
         * Example:@code
         *          math.square
         *          math.multiplication@endcode
         *          
         */
        std::string id_;


        /**
         * Plug-in header is a JSON object containing basic information about
         * the plug-in such as plug-in id, events, what plug-in it extends and
         * plug-in loader.
         * Example of header:@code
         *                    {
         *                      "id"      : "betterworddefinition.word-filter",
         *                      "events"  : ["load-text-after","load-description-after"],
         *                      "extends" : ["worddefinition.word-filter"],
         *                      "loader"  : {}
         *                    }@endcode
         */
        web::json::value header_;


        /**
         * Script/executable to execute, can be a sript or a path to a script or
         * a path to an executable.
         */
        std::string script_;


        /**
         * Plug-in configuration is a JSON object containing the plug-in
         * properties and values needed during the plug-in runtime influencing the
         * plug-in behavior.
         * Example of plug-in configuration:@code
         *                  {
         *                     "url_to_scrap"             : "https://cookinapps.io",
         *                     "maximum_execution_time"  : 3600,
         *                     "output_directory"        : "user/output/directory"
         *                  }@endcode
         */
        web::json::value configuration_;


        /**
         * Runnable status of a plug-in.
         * True if plug-in is runnable, and false
         * if it is not.
         */
        bool runnable_ = true;

        /**
         * Load Plug-in properties.
         * Normally called once, as the properties are
         * statics shared by all plug-ins.
         */
        virtual void LoadProperties(){};


        /**
         * Returns the Plug-in Handler that manages the
         * lifecycle of the plug-in.
         * @return  Plug-in Handler that manages the
         *          lifecycle of the plug-in.
         */
        virtual granada::plugin::PluginHandler* plugin_handler(){
          return nullptr;
        };

    };


    /**
     * Plug-in Factory, used to instanciate Plugins and PluginHandlers.
     */
    class PluginFactory{

      public:
        virtual std::shared_ptr<granada::plugin::Plugin>Plugin(){
          return std::shared_ptr<granada::plugin::Plugin>(new granada::plugin::Plugin());
        };

        virtual std::shared_ptr<granada::plugin::Plugin>Plugin(granada::plugin::PluginHandler* plugin_handler,const std::string& id){
          return std::shared_ptr<granada::plugin::Plugin>(new granada::plugin::Plugin(plugin_handler,id));
        };

        virtual std::shared_ptr<granada::plugin::Plugin>Plugin(granada::plugin::PluginHandler* plugin_handler, const web::json::value& header, const web::json::value& configuration, const std::string& script){
          return std::shared_ptr<granada::plugin::Plugin>(new granada::plugin::Plugin(plugin_handler,header,configuration,script));
        };

        virtual std::shared_ptr<granada::plugin::PluginHandler>PluginHandler(){
          return std::shared_ptr<granada::plugin::PluginHandler>(new granada::plugin::PluginHandler());
        };

        virtual std::shared_ptr<granada::plugin::PluginHandler>PluginHandler(const std::string& id){
          return std::shared_ptr<granada::plugin::PluginHandler>(new granada::plugin::PluginHandler(id));
        };

        virtual std::shared_ptr<granada::plugin::PluginHandler>PluginHandler(granada::plugin::PluginHandler* plugin_handler){
          return std::shared_ptr<granada::plugin::PluginHandler>(plugin_handler);
        };
    };
  }
}
