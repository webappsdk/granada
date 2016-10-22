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
  * Message Controller
  * Used for listing, inserting, editing, deleting users' messages.
  *
  */

#pragma once
#include <unordered_map>
#include "cpprest/json.h"
#include "cpprest/asyncrt_utils.h"
#include "granada/cache/cache_handler.h"
#include "cpprest/http_client.h"
#include "granada/http/parser.h"
#include "granada/http/oauth2/oauth2.h"
#include "granada/http/session/session.h"
#include "granada/http/controller/controller.h"
#include "../../business/message.h"

namespace granada{
  namespace http{
    namespace controller{
      class MessageController : public Controller {
        public:

          /**
           * Constructor
           */
          MessageController(utility::string_t url, std::shared_ptr<granada::http::session::SessionFactory>& session_factory, std::shared_ptr<granada::cache::CacheHandler>& cache);

        private:

          /**
           * Nonce string generator, for generating unique strings tokens.
           * From cpprest/asyncrt_utils.h
           * Generate a nonce string containing random alphanumeric characters (A-Za-z0-9).
           */
          std::unique_ptr<utility::nonce_generator> n_generator_;


          /**
           * Allows to have a unique point for checking and setting sessions.
           * Can be used to create a new session if it does not exist.
           */
          std::shared_ptr<granada::http::session::SessionFactory> session_factory_;


          /**
           * Where the message are going to be stored.
           */
          std::shared_ptr<granada::cache::CacheHandler> cache_;


          /**
           * Handles HTTP PUT requests.
           * @param request HTTP request.
           */
          void handle_put(web::http::http_request request);


          /**
           * Handles HTTP POST requests.
           * @param request HTTP request.
           */
          void handle_post(web::http::http_request request);

          /**
           * Handles HTTP DELETE requests.
           * @param request HTTP request.
           */
          void handle_delete(web::http::http_request request);


          void MessageApplicationSessionFactory(std::unique_ptr<granada::http::session::Session>& session, web::http::http_request request, web::http::http_response response);
      };
    }
  }
}
