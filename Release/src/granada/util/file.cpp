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
  */

#include "granada/util/file.h"

namespace granada{
  namespace util{
    namespace file{

      PropertyFile::PropertyFile(const std::string& file_path){
        properties_ = ParseConfigurationFile(file_path);
      }

      const std::string PropertyFile::GetProperty(const std::string& name){
        auto it = properties_.find(name);
        if (it != properties_.end()){
          return it->second;
        }else{
          return std::string();
        }
      };

      std::unordered_map<std::string,std::string> PropertyFile::ParseConfigurationFile(const std::string &configuration_file_path){
        std::unordered_map<std::string,std::string> properties;

        if (!configuration_file_path.empty()){
          // get configuration file data.
          std::ifstream ifs(configuration_file_path);
          if (ifs.good()){
            std::string configuration( (std::istreambuf_iterator<char>(ifs) ),
                               (std::istreambuf_iterator<char>()) );

            // read configuration string line by line and parse properties.
            std::istringstream f(configuration);
            std::string line;
            while (std::getline(f, line)) {
               ParseProperty(line,properties);
            }
          }
        }
       return properties;
      }

      const bool PropertyFile::ParseProperty(const std::string &line,std::unordered_map<std::string,std::string> &properties){
        // check if line has content.
        if (line.length()>0){
          if(line[0]=='#'){
            // line is a comment.
            return false;
          }else{
            // parse a line into a pair of property name / value and insert it in the properties unordered_map.
            std::size_t pos = line.find("=");
            if (pos!=std::string::npos){
              properties.insert(std::make_pair(line.substr(0,pos),line.substr(pos+1)));
              return true;
            }else{
              // there is no property and value that can be parsed.
              return false;
            }
          }
        }
        // line is empty.
        return false;
      }
    }
  }
}
