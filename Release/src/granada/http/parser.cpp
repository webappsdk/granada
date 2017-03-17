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
#include "granada/http/parser.h"

namespace granada{
  namespace http{
    namespace parser{

      std::unordered_map<std::string, std::string> ParseCookies(const web::http::http_request &request){
        std::unordered_map<std::string, std::string> cookies;
        web::http::http_headers headers = request.headers();

        if( headers.has(utility::conversions::to_string_t(entity_keys::http_parser_cookie)) ){
			    const std::string cookies_str = utility::conversions::to_utf8string(headers[utility::conversions::to_string_t(entity_keys::http_parser_cookie)]);

          // separate different cookies.
          std::stringstream ss(cookies_str);
          std::string cookie_name_and_content;
          while (std::getline(ss, cookie_name_and_content, ';')) {
            granada::util::string::trim(cookie_name_and_content);
            // get cookie name and content.
            const std::string delimiter("=");
            const size_t delimiter_length(delimiter.length());
            auto it = std::search(cookie_name_and_content.begin(), cookie_name_and_content.end(), delimiter.c_str(), delimiter.c_str() + delimiter_length);

            if (it!=cookie_name_and_content.end()){
              // insert cookie name and content in cookies map.
              cookies.insert(std::make_pair(std::string(cookie_name_and_content.begin(),it), std::string(it+delimiter_length,cookie_name_and_content.end())));
            }
          }
        }
        return cookies;
      }


      std::unordered_map<std::string, std::string> ParseQueryString(const std::string& query_string){
        std::unordered_map<std::string, std::string> parsed_query;
        if (!query_string.empty()){
          std::vector<std::string> keys_and_values;
          granada::util::string::split(query_string,'&',keys_and_values);
          int len = keys_and_values.size();
          while (len--) {
            std::vector<std::string> key_and_value;
            granada::util::string::split(keys_and_values[len],'=',key_and_value);
            if (key_and_value.size() > 1){
              parsed_query.insert(std::make_pair(key_and_value[0],utility::conversions::to_utf8string(web::uri::decode(utility::conversions::to_string_t(key_and_value[1])))));
            }
          }
        }
        return parsed_query;
      }


      std::unordered_map<std::string, std::unordered_map<std::string, std::vector<unsigned char>>> ParseMultipartFormData(const web::http::http_request &request){

        std::unordered_map<std::string, std::unordered_map<std::string, std::vector<unsigned char>>> multipart_form_data;

        web::http::http_headers headers = request.headers();

        // get boundary value from headers
        std::string boundary;
        try {
          boundary = ExtractBoundaryMDF(headers);
        }catch(const std::exception e){
          return multipart_form_data;
        }

        // remove headers
        headers.clear();

        // extract body from http_access as a vector of unsigned char
        auto body = request.extract_vector().get();

        if (!body.empty()){
          try{
            while(ParseFieldsAndPropertiesMDF(body,boundary,multipart_form_data));
          }catch(const std::exception e){}
        }

        return multipart_form_data;
      }


      std::string ExtractBoundaryMDF(const web::http::http_headers &headers){
        std::string content_type = utility::conversions::to_utf8string(headers.content_type());
        if (!content_type.empty()){
		      const std::string delimiter(utility::conversions::to_utf8string(entity_keys::http_parser_boundary_delimiter));
          auto pos = content_type.rfind(delimiter);
          if (pos != std::string::npos) {
            content_type.erase(0,pos + delimiter.length());
          }
        }
        return content_type;
      }


      bool ParseFieldsAndPropertiesMDF(std::vector<unsigned char> &body, const std::string &boundary, std::unordered_map<std::string,std::unordered_map<std::string, std::vector<unsigned char>>> &multipart_form_data){
        
        std::unordered_map<std::string, std::vector<unsigned char>> parsed_properties;

        // get block between boundaries.
        const char *boundary_c = boundary.c_str();
        const size_t boundary_length = strlen(boundary_c);
		    const std::vector<unsigned char>::iterator boundary_end_it = std::search(body.begin(), body.end(), boundary_c, boundary_c + boundary_length) + boundary_length;

        // check if this boundary is the last one, in that case that will represent the end of the data.
        const std::string boundary_close(boundary_end_it,boundary_end_it+2);
        if (boundary_close.compare("--") != 0){
          // not the end of the data, this is just another block containing field properties and its value.
          // remove unnecessary boundary information from the request body
		  
		      body.erase(body.begin(), boundary_end_it);

          // extract the different properties from a block with this format: Content-Disposition: form-data; name="file"; filename="pure-html-websites.png"
          // Extract from the first "; " until EOL and then parse into a unordered_map
          auto property_begin_it = GetIteratorMDF("; ", body, true);
		      body.erase(body.begin(),property_begin_it);

          // parse properties into a unordered_map of string and vector of unsigned char.
          auto properties_end_it = GetIteratorMDF("\r\n", body, false);


          std::vector<unsigned char> properties(body.begin(),properties_end_it);
          while(ParsePropertyMDF(properties,parsed_properties));
		  
          // get value of the field and treat it as another property
          auto value_begin_it = GetIteratorMDF("\r\n\r\n", body, true);
          body.erase(body.begin(),value_begin_it);
          auto value_end_it = std::search(body.begin(), body.end(), boundary_c, boundary_c + boundary_length)-8;
          std::vector<unsigned char> value(body.begin(),value_end_it);
          parsed_properties.insert(std::make_pair(utility::conversions::to_utf8string(entity_keys::http_parser_property_value_label), value));

          // insert the unordered_map of properties into the unordered_map of fields
		      std::vector<unsigned char> key_name = parsed_properties[utility::conversions::to_utf8string(entity_keys::http_parser_property_name_label)];
          multipart_form_data.insert(std::make_pair(std::string(key_name.begin(),key_name.end()), parsed_properties));

          return true;
        }

        return false;
      }


      bool ParsePropertyMDF(std::vector<unsigned char> &properties,std::unordered_map<std::string, std::vector<unsigned char>> &parsed_properties){
        // get the name of the property.
        const std::string delimiter("=\"");
        const size_t delimiter_length(delimiter.length());
        auto it = std::search(properties.begin(), properties.end(), delimiter.c_str(), delimiter.c_str() + delimiter_length);

        // remove name of the property from properties vector.
	std::string property_name(properties.begin(), it);
        properties.erase(properties.begin(),it+delimiter_length);
		
        // get the value of the property.
        auto it_end = GetIteratorMDF("\"",properties,false);

        std::vector<unsigned char> property_value(properties.begin(), it_end);
		
        parsed_properties.insert(std::make_pair(property_name, property_value));
		
        // check if there are other properties after the one that has just been parsed, if so return true, if not return false.
    		if (it_end + 1 != properties.end() && it_end + 2 != properties.end() && it_end + 3 != properties.end()){
    			std::string end_check(it_end+1, it_end + 3);
    			if (end_check.compare("; ") == 0){
    				properties.erase(properties.begin(), it_end + 3);
    				return true;
    			}
    		}

        return false;
      }


      std::vector<unsigned char>::iterator GetIteratorMDF(const char *chr,std::vector<unsigned char> &body, const bool end){
        const size_t chr_length = strlen(chr);
        if ( end ){
          return std::search(body.begin(), body.end(), chr, chr + chr_length) + chr_length;
        }else{
          return std::search(body.begin(), body.end(), chr, chr + chr_length);
        }
      }


      std::string ParseURIFromReferer(const web::http::http_request& request){
        const web::http::http_headers& headers = request.headers();
        auto it = headers.find(web::http::header_names::referer);
        if (it != headers.end()){
          web::uri uri(it->second);
          const int port = uri.port();
          return utility::conversions::to_utf8string(uri.scheme() + U("://") + uri.host() + utility::conversions::to_string_t(port > 0 ? ":" + std::to_string(port) : "") + uri.path());
        }
        return std::string();
      }

    }
  }
}
