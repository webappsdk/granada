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
  * Parsers for C++ Rest SDK (Casablanca) HTTP requests.
  */
#pragma once

#include <string>
#include <unordered_map>
#include "cpprest/http_listener.h"

namespace granada{
  namespace http{
    namespace parse{

      /*
       * Parse an http_request with multipart/form data content type into a unordered_map.
       * example:
       * ( HEADERS )
       * (...)
       * ------WebKitFormBoundarymBItcSVphgAjmbJC
       * Content-Disposition: form-data; name="test-text-field"
       *
       * Test value
       * ------WebKitFormBoundarymBItcSVphgAjmbJC
       * Content-Disposition: form-data; name="file"; filename="1194985432884406852bee3_mimooh_01.svg.thumb.png"
       * Content-Type: image/png
       *
       * \89PNG
       * #
       * \00\00\00
       * IHDR\00\00\00d\00\00\00^##\00\00\00\D5#\B1\9F\00\00\00#
       * (…)
       * ------WebKitFormBoundarymBItcSVphgAjmbJC--
       *
       * This http request Will be parsed into a unordered_map of unordered_maps of vectors of unsigned char so we will get:
       *
       * parsed multipart form (unordered_map)
       * 	|_ test-text-field (unordered_map)
       * 	|	|_ value (vector<unsigned char>)
       * 	|
       * 	|_ file (unordered_map)
       * 		|_ value (vector<unsigned char>)
       * 		|_ filename (vector<unsigned char>)
       * 		|_ Content-Type (vector<unsigned char>)
       *
       * @param request HTTP request containing multipart/form data.
       * @return        Parsed multipart/form data content.
       */
      std::unordered_map<std::string, std::unordered_map<std::string, std::vector<unsigned char>>> ParseMultipartFormData(web::http::http_request &request);

      /**
       * Returns the value of the boundary of the multipart/form data
       * @param  headers Headers of the HTTP Request
       * @return         Boundary of the multipart/form data. Example: WebKitFormBoundaryKS484Mi6jduf07q
       */
      std::string ExtractBoundaryMDF(web::http::http_headers &headers);

      /**
       * Fill parsed multi part form data unordered_map with fields, properties and values.
       * @param  body                Non-parsed multipart/form data fields, properties and values.
       * @param  boundary            Boundary of the multipart/form data.
       * @param  multipart_data_form Parsed multipart/form data in unordered_map.
       * @return                     Return true if there are more fields, properties and values after this one.
       *                             Return false if this is there is no more data to parse after.
       */
      bool ParseFieldsAndPropertiesMDF(std::vector<unsigned char> &body, std::string &boundary,std::unordered_map<std::string,std::unordered_map<std::string, std::vector<unsigned char>>> &multipart_data_form);

      /**
       * Parse property with this format: name="file";
       * of a multipart/form data field into key-value pair and
       * insert it into the parsed properties unordered_map.
       * @param  properties        Non-parsed properties
       * @param  parsed_properties Parsed properties.
       * @return                   True if there is another property after it,
       *                           False if there is no other property to parse after it.
       */
      bool ParsePropertyMDF(std::vector<unsigned char> &properties,std::unordered_map<std::string, std::vector<unsigned char>> &parsed_properties);

      /**
       * Search the position of first appearance of chr in body vector and returns the position iterator of the begining or the end.
       * @param  chr  Text to search.
       * @param  body Contains all the data where we want to search chr.
       * @param  end  If true extract the iterator corresponding to the position after the first appearance of chr in body,
       *              If false before the first appearance of chr.
       * @return      Iterator of the begining or the end of the first appearance of chr.
       */
      std::vector<unsigned char>::iterator GetIteratorMDF(const char *chr, std::vector<unsigned char> &body, const bool end);
    }
  }
}
