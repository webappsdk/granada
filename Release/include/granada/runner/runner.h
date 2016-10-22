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
  * Interface for running scripts or executables.
  */

#pragma once
#include <string>
#include "granada/functions.h"

namespace granada{

  /**
   * Namespace containing runners for running script or executables.
   */
  namespace runner{

    /**
     * Interface for running scripts or executables.
     */
    class Runner
    {
      public:

        /**
         * Constructor
         */
        Runner(){};


        /**
         * Destructor
         */
        virtual ~Runner(){};
        

        /**
         * Run script or executable.
         * 
         * @param script  Script/executable or path to script/executable
         * @return        Result or response in form of string.
         */
        virtual std::string Run(const std::string& script){ return std::string(); };
        

        /**
         * Returns a pointer to the collection of functions
         * that can be called from the script/executable.
         * @return  Pointer to the collection of functions
         *          that can be called from the script/executable.
         */
        virtual std::shared_ptr<granada::Functions> functions(){
          return std::shared_ptr<granada::Functions>(nullptr);
        };


        /**
         * Returns a vector with the extensions of the scripts/executables
         * Extensions examples: ["js"], ["sh"], ["exe"], ["js","sh"]
         * @return   Vector with the extensions.
         */
        virtual std::vector<std::string> extensions(){
          return std::vector<std::string>();
        };
    };
  }
}
