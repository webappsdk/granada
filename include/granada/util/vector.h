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
  * Utils for vector manipulation.
  */

#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <iterator>
#include <iostream>

namespace granada{
  namespace util{

    /**
     * Utils for vector manipulation.
     */
    namespace vector{

      /**
       * Convert a vector of strings into a string of elements separated by given delimiter.
       * Example:
       * 		["hello","world","John","Doe"] ==[strigify with delimiter = ',']==> hello,world,John,Doe
       * @param  elems Vector of strings to stringify.
       * @param  delim Delimiter that will be between the elements in the resultant string.
       * @return       Stringified vector.
       */
      static std::string stringify(const std::vector<std::string> &elems, const char* delim){
        std::ostringstream oss;
        if (!elems.empty())
        {
          std::copy(elems.begin(), elems.end()-1,
          std::ostream_iterator<std::string>(oss, delim));
          oss << elems.back();
        }
        return oss.str();
      }
    }
  }
}
