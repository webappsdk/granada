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
  * Utils for json manipulation.
  */

#pragma once
#include <string>
#include "cpprest/json.h"

namespace granada{
  namespace util{

    /**
     * Utils for json manipulation.
     */
    namespace json{
      

      /**
       * Parse a JSON value into string and returns the string, if the JSON value
       * is not a string, returns an empty string.
       * @param   json  JSON value to parse to string.
       * @return        String or empty string.
       */
      static inline std::string as_string(const web::json::value& json){
        if (!json.is_null() && json.is_string()){
          return json.as_string();
        }
        return std::string();
      };

      /**
       * Parse a JSON value field into string and returns the string, if the JSON value field
       * is not a string, returns an empty string.
       * @param   json  JSON value containing the field to parse to string.
       * @param   key   Key of the field to parse to string.
       * @return        String or empty string.
       */
      static inline std::string as_string(const web::json::value& json, const std::string& key){
        if (!json.is_null() && json.is_object() && json.has_field(key)){
          web::json::value str_json = json.at(key);
          return as_string(str_json);
        }
        return std::string();
      };


      /**
       * Parse a JSON value field into JSON object, if the JSON value field
       * is not a JSON object, returns an empty JSON object {}.
       * @param   json  JSON value containing the field to parse to JSON object.
       * @param   key   Key of the field to parse to JSON object.
       * @return        JSON object or empty JSON object.
       */
      static inline web::json::value as_object(const web::json::value& json, const std::string& key){
        if (!json.is_null() && json.is_object() && json.has_field(key)){
          web::json::value json_object = json.at(key);
          if (json_object.is_object()){
            return json_object;
          }
        }
        return web::json::value::object();
      };


      /**
       * Parse a JSON value field into JSON array, if the JSON value field
       * is not a JSON array, returns an empty JSON array [].
       * @param   json  JSON value containing the field to parse to JSON array.
       * @param   key   Key of the field to parse to JSON array.
       * @return        JSON array or empty JSON array.
       */
      static inline web::json::value as_array(const web::json::value& json, const std::string& key){
        if (!json.is_null() && json.is_object() && json.has_field(key)){
          web::json::value json_array = json.at(key);
          if (json_array.is_array()){
            return json_array;
          }
        }
        return web::json::value::array();
      };


      /**
       * Returns the first element of a JSON object or a JSON array.
       * If there is no first element returns a null json value.
       * @param   json  JSON to extract the first element.
       * @return        First element, JSON value.
       */
      static web::json::value first(const web::json::value& json){
        if (json.size() > 0){
          if (json.is_object()){
            for(auto it = json.as_object().cbegin(); it != json.as_object().cend(); ++it){
              return it->second;
            }
          }else if (json.is_array()){
            return json.at(0);
          }
        }
        web::json::value null_json_value;
        return null_json_value;
      };

    }
  }
}
