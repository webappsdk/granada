/**
  * Copyright (c) <2016> Web App SDK <support@htmlpuzzle.net>
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
#include "parser.h"

namespace granada{
  namespace http{
    namespace parser{

      std::unordered_map<std::string, std::unordered_map<std::string, std::vector<unsigned char>>> ParseMultipartFormData(web::http::http_request &request){

        std::unordered_map<std::string, std::unordered_map<std::string, std::vector<unsigned char>>> multipart_form_data;

        web::http::http_headers headers = request.headers();

        // get boundary value from headers
        std::string boundary;
        try {
          boundary = ExtractBoundaryMDF(headers);
        }catch(const std::exception& e){
          return multipart_form_data;
        }

        // remove headers
        headers.clear();

        // extract body from http_access as a vector of unsigned char
        auto body = request.extract_vector().get();

        try{
          while(ParseFieldsAndPropertiesMDF(body,boundary,multipart_form_data));
        }catch(const std::exception& e){
          return multipart_form_data;
        }

        return multipart_form_data;
      }


      std::string ExtractBoundaryMDF(web::http::http_headers &headers){
        std::string content_type = headers.content_type();
        if (!content_type.empty()){
          std::string delimiter("; boundary=----");
          auto pos = content_type.rfind(delimiter);
          if (pos != std::string::npos) {
            content_type.erase(0,pos + delimiter.length());
          }
        }
        return content_type;
      }


      bool ParseFieldsAndPropertiesMDF(std::vector<unsigned char> &body, std::string &boundary, std::unordered_map<std::string,std::unordered_map<std::string, std::vector<unsigned char>>> &multipart_form_data){
        std::unordered_map<std::string, std::vector<unsigned char>> parsed_properties;

        // get block between boundaries.
        const char *boundary_c = boundary.c_str();
        size_t boundary_length = strlen(boundary_c);
        auto boundary_end_it = std::search(body.begin(), body.end(), boundary_c, boundary_c + boundary_length) + boundary_length;

        // check if this boundary is the last one, in that case that will represent the end of the data.
        std::string boundary_close(boundary_end_it,boundary_end_it+2);
        if (boundary_close.compare("--") != 0){
          // not the end of the data, this is just another block containing field properties and its value.
          // remove unnecessary boundary information from the request body
          body.assign(boundary_end_it, body.end());

          // extract the different properties from a block with this format: Content-Disposition: form-data; name="file"; filename="pure-html-websites.png"
          // Extract from the first "; " until EOL and then parse into a unordered_map
          auto property_begin_it = GetIteratorMDF("; ", body, true);
          body.assign(property_begin_it, body.end());

          // parse properties into a unordered_map of string and vector of unsigned char.
          auto properties_end_it = GetIteratorMDF("\r\n", body, false);
          std::vector<unsigned char> properties(body.begin(),properties_end_it);
          while(ParsePropertyMDF(properties,parsed_properties));

          // get value of the field and treat it as another property
          auto value_begin_it = GetIteratorMDF("\r\n\r\n", body, true);
          body.assign(value_begin_it, body.end());
          auto value_end_it = std::search(body.begin(), body.end(), boundary_c, boundary_c + boundary_length)-8;
          std::vector<unsigned char> value(body.begin(),value_end_it);
          parsed_properties.insert(std::make_pair("value", value));

          // insert the unordered_map of properties into the unordered_map of fields
          std::vector<unsigned char> key_name = parsed_properties["name"];
          std::string key(key_name.begin(),key_name.end());
          multipart_form_data.insert(std::make_pair(key, parsed_properties));

          return true;
        }

        return false;
      }


      bool ParsePropertyMDF(std::vector<unsigned char> &properties,std::unordered_map<std::string, std::vector<unsigned char>> &parsed_properties){
        // get the name of the property.
        const char *delimiter = "=\"";
        size_t delimiter_length = strlen(delimiter);
        auto it = std::search(properties.begin(), properties.end(), delimiter, delimiter + delimiter_length);
        std::string property_name(properties.begin(),it);

        // remove name of the property from properties vector.
        properties.assign(it+delimiter_length,properties.end());

        // get the value of the property.
        auto it_end = GetIteratorMDF("\"",properties,false);
        std::vector<unsigned char> property_value(properties.begin(), it_end);

        parsed_properties.insert(std::make_pair(property_name, property_value));

        // check if there are other properties after the one that has just been parsed, if so return true, if not return false.
        std::string end_check(it_end+1,it_end+3);
        if (end_check.compare("; ") == 0){
          properties.assign(it_end+3, properties.end());
          return true;
        }

        return false;
      }


      std::vector<unsigned char>::iterator GetIteratorMDF(const char *chr,std::vector<unsigned char> &body, const bool end){
        size_t chr_length = strlen(chr);
        if ( end ){
          return std::search(body.begin(), body.end(), chr, chr + chr_length) + chr_length;
        }else{
          return std::search(body.begin(), body.end(), chr, chr + chr_length);
        }
      }
    }
  }
}
