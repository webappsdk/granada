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
  * Auth Controller
  * Resource Owner Password Credentials Grant Controller
  *
  */

#pragma once
#include <unordered_map>
#include "granada/util/string.h"
#include "granada/http/parser.h"
#include "granada/http/controller/controller.h"
#include "granada/http/session/redis_storage_session.h"
#include "../../business/user.h"

namespace granada{
  namespace http{
    namespace controller{
      class AuthController : public Controller {
        public:

          /**
           * Constructor
           */
          AuthController(utility::string_t url);

        private:

          /**
           * Handles HTTP POST requests.
           * @param request HTTP request.
           */
          void handle_post(http_request request);
      };
    }
  }
}
