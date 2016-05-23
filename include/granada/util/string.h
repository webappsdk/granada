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
  * Utils for string manipulation.
  */

#pragma once

#include <algorithm>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>

namespace granada{
  namespace util{
    namespace string{

      /**
       * Trim from start
       * @param s String to ltrim.
       */
      static inline void ltrim(std::string &s) {
          s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
      }

      /**
       * Trim from end
       * @param s String to rtrim.
       */
      static inline void rtrim(std::string &s) {
          s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
      }

      /**
       * Trim from both ends
       * @param s String to trim.
       */
      static inline void trim(std::string &s) {
          ltrim(s);
          rtrim(s);
      }

      /**
       * Split a string into a vector with a given delimiter.
       * @param   s     String to split.
       * @param   delim Delimiter.
       * @param   elems Vector containing the splitted string.
       * @return        Vector containing the splitted string.
       */
      static void split(const std::string &s, char delim, std::vector<std::string> &elems) {
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
            elems.push_back(item);
        }
      }

      /**
       * Replace a map of key tags in a string by a value.
       * Example:
       * 			content:
       * 					hello {{username}} !!! {{date}}
       * 			Replace map:
       * 					username  => "John Doe"
       * 					date      => "Tuesday, May 17, 2016"
       * 		  Open: {{ , Close: }}
       * 			Result:
       * 					hello John Doe !!! Tuesday, May 17, 2016
       *
       * @param  content   Content containing the tags to replace.
       * @param  values    Map with the key-values pairs containing the
       *                   values to replace in the file.
       * @param  open      Before tag mark.
       * @param  close     After tag mark.
       */
      static void replace(std::string& content,const std::unordered_map<std::string,std::string>& values, const std::string& open, const std::string& close){
        std::string tag;
        std::string value;
        for (auto it = values.begin(); it != values.end(); ++it){
          tag.assign(open + it->first + close);
          value = it->second;
          size_t pos = 0;
          while ((pos = content.find(tag, pos)) != std::string::npos) {
               content.replace(pos, tag.length(), value);
               pos += value.length();
          }
        }
      }


      /**
       * Replace a map of key tags in a string by a value using before and after
       * tag marks: open "{{" and default close "}}"
       * @param content ontent containing the tags to replace.
       * @param values  Map with the key-values pairs containing the
       *                values to replace in the file.
       */
      static inline void replace(std::string& content,const std::unordered_map<std::string,std::string>& values){
        granada::util::string::replace(content,values,"{{","}}");
      }
    }
  }
}
