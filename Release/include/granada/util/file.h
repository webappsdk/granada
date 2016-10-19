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
  * Collection of functions and classes related to file information or manipulation.
  *
  */

#pragma once

#include <string>
#include <vector>
#include <deque>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include "string.h"

namespace granada{
  namespace util{

    /**
     * Collection of functions and classes related to file information or manipulation.
     */
    namespace file{

      /**
       * Return the extension of a file based on the given filename or path.
       * @param  filename File name or path from which to extract the extension.
       * @return          Extension of the file.
       */
      static const std::string GetExtension(const std::string& filename){
        // extract the extension of the file from the filename.
        const std::size_t& found = filename.find_last_of(".");
        if (found != std::string::npos){
          const std::string& extension = filename.substr(found,filename.length());
          if (!extension.empty()){
            return extension.substr(1);
          }
        }
        return std::string();
      }

      /**
       * Returns the content of the file with given path in form of string.
       * @param   file_path   Path of the file.
       * @return              Content of the file (string).
       */
      static inline std::string ContentAsString(const std::string& file_path){
        if (!file_path.empty()){
          std::ifstream ifs(file_path);
          if (ifs.good()){
            return std::string((std::istreambuf_iterator<char>(ifs)),(std::istreambuf_iterator<char>()));
          }
        }
        return std::string();
      }

      /**
       * Returns the content of the file with given path in form of JSON value.
       * @param   file_path   Path of the file.
       * @return              Content of the file (JSON object or JSON array).
       */
      static inline web::json::value ContentAsJSON(const std::string& file_path){
        if (!file_path.empty()){
          std::ifstream ifs(file_path);
          if (ifs.good()){
            return granada::util::string::to_json(std::string((std::istreambuf_iterator<char>(ifs)),(std::istreambuf_iterator<char>())));
          }
        }
        return web::json::value::object();
      }


      /**
       * Replace a deque of key tags in a file by a value.
       * Example:
       * 			file:
       * 					hello {{username}} !!! {{date}}
       * 			Replace map:
       * 					username  => "John Doe"
       * 					date      => "Tuesday, May 17, 2016"
       * 		  Open: {{ , Close: }}
       * 			Result:
       * 					hello John Doe !!! Tuesday, May 17, 2016
       *
       * @param  file_path Path of the file containing the tags to replace.
       * @param  values    Deque with the key-values pairs containing the
       *                   values to replace in the file.
       * @param  open      Before tag mark.
       * @param  close     After tag mark.
       * @return           Content of the file with values replaced.
       */
      static const std::string Replace(const std::string& file_path,const std::deque<std::pair<std::string,std::string>>& values, const std::string& open, const std::string& close){
        if (!file_path.empty()){
          std::ifstream ifs(file_path);
          if(ifs.good()){
            std::string content( (std::istreambuf_iterator<char>(ifs) ),
                               (std::istreambuf_iterator<char>()));
            // replace tags in content by values in map.
            granada::util::string::replace(content,values,open,close);
            return content;
          }
        }
        return std::string();
      }


      /**
       * Replace a map of key tags in a file by a value.
       * Example:
       *      file:
       *          hello {{username}} !!! {{date}}
       *      Replace map:
       *          username  => "John Doe"
       *          date      => "Tuesday, May 17, 2016"
       *      Open: {{ , Close: }}
       *      Result:
       *          hello John Doe !!! Tuesday, May 17, 2016
       *
       * @param  file_path Path of the file containing the tags to replace.
       * @param  values    Map with the key-values pairs containing the
       *                   values to replace in the file.
       * @param  open      Before tag mark.
       * @param  close     After tag mark.
       * @return           Content of the file with values replaced.
       */
      static const std::string Replace(const std::string& file_path,const std::unordered_map<std::string,std::string>& values, const std::string& open, const std::string& close){
        if (!file_path.empty()){
          std::ifstream ifs(file_path);
          if(ifs.good()){
            std::string content( (std::istreambuf_iterator<char>(ifs) ),
                               (std::istreambuf_iterator<char>()));
            // replace tags in content by values in map.
            granada::util::string::replace(content,values,open,close);
            return content;
          }
        }
        return std::string();
      }


      /**
       * Same as Replace: Replace a deque of key tags in a file by a value
       * but use default before and after tag marks: "{{" and "}}".
       * @param  file_path Path of the file containing the tags to replace.
       * @param  values    Deque with the key-values pairs containing the
       *                   values to replace in the file.
       * @return           Content of the file with values replaced.
       */
      static inline const std::string Replace(const std::string& file_path,const std::deque<std::pair<std::string,std::string>>& values){
        return granada::util::file::Replace(file_path,values,"{{","}}");
      }


      /**
       * Same as Replace: Replace a map of key tags in a file by a value
       * but use default before and after tag marks: "{{" and "}}".
       * @param  file_path Path of the file containing the tags to replace.
       * @param  values    Map with the key-values pairs containing the
       *                   values to replace in the file.
       * @return           Content of the file with values replaced.
       */
      static inline const std::string Replace(const std::string& file_path,const std::unordered_map<std::string,std::string>& values){
        return granada::util::file::Replace(file_path,values,"{{","}}");
      }


      /**
       * Contains methods for parsing a property file and
       * getting the parsed properties.
       *
       * Example of property file:
       *
       * 				############################
       * 				# EXAMPLE OF PROPERTY FILE #
       * 				############################
       * 				# comment on property1
       * 				property1=value1
       *
       * 				# comment on property2
       * 				property2=value2
       */
      class PropertyFile
      {
        public:
          /**
           * Constructor
           * @param file_path Path of the file to parse.
           */
          PropertyFile(const std::string& file_path);


          /**
           * Get a property from the property file.
           * @param  name [description]
           * @return      [description]
           */
          const std::string GetProperty(const std::string& name);

          /**
           * Check if file properties are parsed
           */
          const bool empty(){ return properties_.empty(); };
        private:


          /**
           * Parse a configuration file into a unordered_map of property names and values.
           *
           * Example:
           *
           * 				############################
           * 				# EXAMPLE OF PROPERTY FILE #
           * 				############################
           * 				# comment on property1
           * 				property1=value1
           *
           * 				# comment on property2
           * 				property2=value2
           *
           * Will be parsed into an unordered_map:
           * 				propery1     => value1
           * 				property2    => value2
           *
           * @param configuration_file_path Path of the configuration file.
           * @return                        unordered_map of property names and values.
           */
          std::unordered_map<std::string,std::string> ParseConfigurationFile(const std::string &configuration_file_path);


          /**
           * Parse a string containing a property name and its value (ex: port=80) into a pair of key/value and insert it in the properties unordered_map.
           * @param  line       String containing a property name and a property value separated by an "=". Ex: port=80
           * @param  properties unordered_map containing pairs of properties and values.
           * @return            True if property has been parsed, false if not or the line to parse does not contain a property name and value.
           */
          const bool ParseProperty(const std::string &line,std::unordered_map<std::string,std::string> &properties);


          /**
           * Parsed properties.
           */
          std::unordered_map<std::string,std::string> properties_;
      };

    }
  }
}
