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
#include <unordered_map>
#include <fstream>
#include <sstream>

namespace granada{
  namespace util{
    namespace file{

      /**
       * Return the extension of a file based on the given filename or path.
       * @param  filename File name or path from which to extract the extension.
       * @return          Extension of the file.
       */
      const std::string GetExtension(const std::string& filename);


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
