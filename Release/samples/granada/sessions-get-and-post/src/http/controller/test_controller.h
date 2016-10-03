/**
  * Copyright (c) <2016> HTML Puzzle Team <htmlpuzzleteam@gmail.com>
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

#pragma once
#include <unordered_map>
#include "granada/http/parser.h"
#include "granada/http/controller/controller.h"
#include "granada/http/session/storage_session.h"

namespace granada{
  namespace http{
    namespace controller{
      class TestController : public Controller {
        public:

          /**
           * Constructor
           */
          TestController(utility::string_t url);

        private:

          /**
           * Handles HTTP GET requests.
           * @param request HTTP request.
           */
          void handle_get(http_request request);


          /**
           * Handles HTTP PUT requests.
           * @param request HTTP request.
           */
          void handle_put(http_request request);


          /**
           * Handles HTTP POST requests.
           * @param request HTTP request.
           */
          void handle_post(http_request request);

          /**
           * Handles HTTP DELETE requests.
           * @param request HTTP request.
           */
          void handle_delete(http_request request);
      };
    }
  }
}
