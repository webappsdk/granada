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
  * Application Controller
  * Used for listing, inserting, editing, deleting users' messages.
  *
  */

#pragma once
#include <unordered_map>
#include "cpprest/json.h"
#include "cpprest/http_client.h"
#include "cpprest/oauth2.h"
#include "granada/util/file.h"
#include "granada/http/oauth2/oauth2.h"
#include "granada/http/parser.h"
#include "granada/http/controller/controller.h"
#include "granada/http/session/redis_storage_session.h"
#include "cpprest/asyncrt_utils.h"
#include "../../business/message.h"
#include "boost/filesystem.hpp"

namespace granada{
  namespace http{
    namespace controller{
      class ApplicationController : public Controller {
        public:

          /**
           * Constructor
           */
          ApplicationController(utility::string_t url, std::shared_ptr<granada::http::session::Checkpoint>& session_checkpoint);

        private:

          std::shared_ptr<std::string> readcode_client_id_;

          std::shared_ptr<std::string> readcode_client_secret_;

          std::shared_ptr<std::string> readwritecode_client_id_;

          std::shared_ptr<std::string> readwritecode_client_secret_;

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
          std::shared_ptr<granada::http::session::Checkpoint> session_checkpoint_;


          /**
           * Handles HTTP GET requests.
           * @param request HTTP request.
           */
          void handle_get(web::http::http_request request);


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


          void MessageApplicationSessionCheckpoint(std::shared_ptr<granada::http::session::Session>& redis_storage_session, web::http::http_request request, web::http::http_response response);

          std::string GetClientId(const std::string& name);

          std::string GetApplicationName(const std::string& name);

          std::string GetClientSecret(const std::string& name);

          std::string GetAccessToken(const std::string& name, std::shared_ptr<granada::http::session::Session>& redis_storage_session);

          std::string GetRoles(const std::string& name);

          std::string ResourceServerRequest(web::http::http_request& request, const std::string& method, const std::string& access_token, const std::string& adress, std::string& error, std::string& error_description);
      };
    }
  }
}
