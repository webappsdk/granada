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
  * Redis Plugin: Defined structure to extend an application using Redis as a
  * cache to store data and Mozilla JavaScript engine "Spidermonkey"
  * to run the plug-ins scripts.
  *
  */

#pragma once
#include "granada/plugin/spidermonkey_plugin.h"
#include "granada/cache/redis_cache_driver.h"


namespace granada{

  namespace plugin{

    /**
     * Plugin Handler: Handles the lifecycle and communication of server side plugins.
     * Manages plug-ins lifecycle: Loads and adds plug-ins, runs plug-ins and removes them.
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
     * std::unique_ptr<granada::plugin::Plugin> plugin = plugin_handler->GetPluginById("product.tax");
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
    class RedisSpidermonkeyPluginHandler : public SpidermonkeyPluginHandler{
      public:

        /**
         * Constructor.
         * Plug-in handler without an id.
         * Load Plug-in Handler properties.
         */
        RedisSpidermonkeyPluginHandler(){
          RedisSpidermonkeyPluginHandler::load_properties_call_once_.call([this](){
            this->LoadProperties();
          });
        };


        /**
         * Constructor.
         * Assigns an id to the plug-in handler.
         * Load Plug-in Handler properties.
         * 
         * @param   id  Unique Identifier of the PluginHandler.
         */
        RedisSpidermonkeyPluginHandler(const std::string id){
          id_ = std::move(id);
          RedisSpidermonkeyPluginHandler::load_properties_call_once_.call([this](){
            this->LoadProperties();
          });
          RedisSpidermonkeyPluginHandler::functions_to_runner_call_once_.call([this](){
            this->AddFunctionsToRunner();
          });
        };


        /**
         * Destructor.
         */
        virtual ~RedisSpidermonkeyPluginHandler(){};


        /**
         * Returns a pointer to the Cache Handler. Used to cache plug-ins headers, loaders, configuration and
         * script paths as well as plug-ins global values. Needs to be overridden.
         * @return  Pointer to the Cache Handler.
         */
        virtual granada::cache::CacheHandler* cache() override{
          return RedisSpidermonkeyPluginHandler::cache_.get();
        };


        /**
         * Returns a pointer to a Plug-in Factory. Used to create PluginHandlers and Plugins.
         * @return Pointer to a plug-in Factory.
         */
        virtual granada::plugin::PluginFactory* plugin_factory() override{
          return RedisSpidermonkeyPluginHandler::plugin_factory_.get();
        };


        /**
         * Returns a pointer to the responsible of running or executing the
         * plug-in scripts/executables.
         * @return Pointer to the scripts/executables runner.
         */
        virtual granada::runner::Runner* runner() override{
          return RedisSpidermonkeyPluginHandler::runner_.get();
        };


      protected:


        /**
         * Pointer to the Cache Handler. Used to cache plug-ins headers, loaders,
         * configuration and script paths as well as plug-ins global values.
         */
        static std::unique_ptr<granada::cache::CacheHandler> cache_;


        /**
         * Pointer to a Plug-in Factory. Used to create PluginHandlers and Plugins.
         */
        static std::unique_ptr<granada::plugin::PluginFactory> plugin_factory_;


        /**
         * Pointer to the responsible of running or executing the
         * plug-in scripts/executables.
         */
        static std::unique_ptr<granada::runner::Runner> runner_;

    };


    /**
     * Extension of the server application. Plug-ins can also extend other plug-ins.
     * Not all of them may be executed, some can just have useful functions or wait
     * to be extended by others. They can communicate. 
     */
    class RedisSpidermonkeyPlugin : public SpidermonkeyPlugin{
      public:
        
        /**
         * Constructor, load plug-in properties.
         */
        RedisSpidermonkeyPlugin(){
          RedisSpidermonkeyPlugin::load_properties_call_once_.call([this](){
            this->LoadProperties();
          });
        };
        

        /**
         * Constructor, set Plug-in Handler id and plug-in id
         * and load properties.
         * 
         * @param plugin_handler      Plug-in Handler.
         * @param id                  Id of the Plug-in. 
         */
        RedisSpidermonkeyPlugin(granada::plugin::PluginHandler* plugin_handler,const std::string id){
          plugin_handler_ = plugin_handler;
          id_ = std::move(id);
          RedisSpidermonkeyPlugin::load_properties_call_once_.call([this](){
            this->LoadProperties();
          });
        };


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
        RedisSpidermonkeyPlugin(granada::plugin::PluginHandler* plugin_handler, const web::json::value header, const web::json::value configuration, const std::string script){
          header_ = std::move(header);
          id_ = granada::util::json::as_string(header, entity_keys::plugin_header_id);
          configuration_ = std::move(configuration);
          plugin_handler_ = plugin_handler;
          script_ = std::move(script);
          RedisSpidermonkeyPlugin::load_properties_call_once_.call([this](){
            this->LoadProperties();
          });
        };


      protected:


        /**
         * Pointer to the Plug-in Handler that manages the
         * lifecycle of the plug-in.
         * @return  Pointer to the Plug-in Handler that manages the
         *          lifecycle of the plug-in.
         */
        granada::plugin::PluginHandler* plugin_handler_;


        /**
         * Returns the Plug-in Handler that manages the
         * lifecycle of the plug-in.
         * @return  Plug-in Handler that manages the
         *          lifecycle of the plug-in.
         */
        virtual granada::plugin::PluginHandler* plugin_handler() override {
          return plugin_handler_;
        }; 

    };


    /**
     * Plug-in Factory, used to instanciate Plugins and PluginHandlers.
     */
    class RedisSpidermonkeyPluginFactory : public SpidermonkeyPluginFactory{

      public:

        virtual std::unique_ptr<granada::plugin::Plugin>Plugin_unique_ptr() override {
          return granada::util::memory::make_unique<granada::plugin::RedisSpidermonkeyPlugin>();
        };

        virtual std::unique_ptr<granada::plugin::Plugin>Plugin_unique_ptr(granada::plugin::PluginHandler* plugin_handler,const std::string& id) override {
          return granada::util::memory::make_unique<granada::plugin::RedisSpidermonkeyPlugin>(plugin_handler,id);
        };

        virtual std::unique_ptr<granada::plugin::Plugin>Plugin_unique_ptr(granada::plugin::PluginHandler* plugin_handler, const web::json::value& header, const web::json::value& configuration, const std::string& script) override {
          return granada::util::memory::make_unique<granada::plugin::RedisSpidermonkeyPlugin>(plugin_handler,header,configuration,script);
        };

        virtual std::unique_ptr<granada::plugin::PluginHandler>PluginHandler_unique_ptr() override {
          return granada::util::memory::make_unique<granada::plugin::RedisSpidermonkeyPluginHandler>();
        };

        virtual std::unique_ptr<granada::plugin::PluginHandler>PluginHandler_unique_ptr(const std::string& id) override {
          return granada::util::memory::make_unique<granada::plugin::RedisSpidermonkeyPluginHandler>(id);
        };

    };
  }
}
