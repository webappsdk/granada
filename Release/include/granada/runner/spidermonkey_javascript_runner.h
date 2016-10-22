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
  * Run javascript using Mozilla's JavaScript engine: Spider Monkey.
  * Spider monkey version 38 is used.
  * https://developer.mozilla.org/en-US/docs/Mozilla/Projects/SpiderMonkey
  */
#pragma once
#include <mutex>
#include <map>
#include "cpprest/json.h"
#include "granada/defaults.h"
#include "runner.h"
#include "jsapi.h"

using namespace JS;

namespace granada{
  namespace runner{

    /**
     * Run javascript using Mozilla's JavaScript engine: Spider Monkey.
     * Spider monkey version 38 is used.
     * https://developer.mozilla.org/en-US/docs/Mozilla/Projects/SpiderMonkey
     * 
     * Recommendation: read Mozilla performance docummentation and specialy
     * the Memory profiling and leak detection tools
     * https://developer.mozilla.org/en-US/docs/Mozilla/Performance
     * 
     * If you are using Valgrind to detect memory leaks please read:
     * https://developer.mozilla.org/en-US/docs/Mozilla/Testing/Valgrind
     */
    class SpiderMonkeyJavascriptRunner : public Runner
    {

      public:

        /**
         * Constructor.
         */
        SpiderMonkeyJavascriptRunner(){
          Init();
        };


        /**
         * Destructor.
         */
        virtual ~SpiderMonkeyJavascriptRunner(){
          Destroy();
        };


        /**
         * @override
         * Run given javascript script and returns the return/response
         * of the script in form of string.
         * 
         * @param _script   Javascript script.
         * @return          Return/Response returned by the script run.
         */
        std::string Run(const std::string& _script);


        /**
         * Returns a pointer to the collection of functions
         * that can be called from the script/executable.
         * @return  Pointer to the collection of functions
         *          that can be called from the script/executable.
         */
        virtual std::shared_ptr<granada::Functions> functions(){
          return SpiderMonkeyJavascriptRunner::functions_;
        };

        
        /**
         * Returns a vector with the extensions of the scripts/executables
         * Extensions examples: ["js"], ["sh"], ["exe"], ["js","sh"]
         * @return   Vector with the extensions.
         */
        virtual std::vector<std::string> extensions(){
          return SpiderMonkeyJavascriptRunner::extensions_;
        };


      protected:


        /* The class of the global object. */
        static JSClass global_class_;


        /**
         * Pointer to the collection of the c++ functions
         * that can be called from the javascript.
         */
        static std::shared_ptr<granada::Functions> functions_;


        /**
         * Array with the extensions of the scripts/executables,
         * will be inserted in the extensions_ vector.
         * Extensions examples: ["js"], ["sh"], ["exe"], ["js","sh"]
         */
        static std::string extensions_arr_[1];


        /**
         * Vector with the extensions of the scripts/executables,
         * its content comes from the extensions_arr_ array.
         * Extensions examples: ["js"], ["sh"], ["exe"], ["js","sh"]
         */
        static std::vector<std::string> extensions_;


        /**
         * Runner initialization error stringified json.
         * Used to respond in the Run functions when there is
         * such error.
         */
        const std::string runner_initialization_error_ = "{\"" + default_strings::runner_error + "\":\"" + default_errors::runner_initialization_error + "\"}";


        /**
         * Initializes the JS engine so that further operations can be performed.
         * It is currently not possible to initialize this runner multiple times
         * without calling Destroy() method.
         */
        void Init();


        /**
         * Free all resources used by the JS engine,
         * not associated with specific runtimes.
         */
        void Destroy();


        /**
         * Wraps the c++ functions that called from the javascript, so the arguments
         * are parsed to a web::json::value and the return to the javascript is
         * also a JSON object.
         * 
         * @param cx    Mozilla spidermonkey JSContext. A context can run scripts.
         *              https://developer.mozilla.org/en-US/docs/Mozilla/Projects/SpiderMonkey/JSAPI_reference/JSRuntime
         * @param argc  Number of argument. (2nd argument of JSNative).
         * @param vp    A pointer to the argument value array.
         *              Arguments that the script is passing to the function, includes the
         *              name of the function to call. See functions() to retrieve the
         *              functions collection.
         * @return      False if there has been an error, true, if everything went OK.
         */
        static bool FunctionWrapper(JSContext *cx, unsigned argc, JS::Value *vp);



    };
  }
}
