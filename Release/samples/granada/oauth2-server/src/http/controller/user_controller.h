/**
  * Copyright (c) <2016> granada <afernandez@cookinapps.io>
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
  * User Controller
  * Used for registering users.
  *
  */

#pragma once
#include <deque>
#include <unordered_map>
#include "cpprest/json.h"
#include "granada/util/string.h"
#include "granada/http/oauth2/oauth2.h"
#include "granada/http/parser.h"
#include "granada/http/controller/controller.h"

namespace granada{
  namespace http{
    namespace controller{
      class UserController : public Controller {
        public:

          /**
           * Constructor
           */
          UserController(utility::string_t url, std::shared_ptr<granada::http::oauth2::OAuth2Factory>& oauth2_factory);

        private:

          std::shared_ptr<granada::http::oauth2::OAuth2Factory> oauth2_factory_;

          /**
           * Handles HTTP POST requests.
           * @param request HTTP request.
           */
          void handle_post(http_request request);

      };
    }
  }
}
