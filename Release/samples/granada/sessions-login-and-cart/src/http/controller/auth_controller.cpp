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

#include "auth_controller.h"

namespace granada{
  namespace http{
    namespace controller{
      AuthController::AuthController(utility::string_t url)
      {
        m_listener_ = std::unique_ptr<http_listener>(new http_listener(url));
        m_listener_->support(methods::POST, std::bind(&AuthController::handle_post, this, std::placeholders::_1));
      }

      void AuthController::handle_post(web::http::http_request request)
      {
        web::http::http_response response;
        granada::http::session::MapSession simple_session(request,response);
        auto paths = uri::split_path(uri::decode(request.relative_uri().path()));
        if (!paths.empty()){
          std::string name = paths[0];

          // returns the status of te session,
          // 0 if user is not logged in
          // 1 if user is logged.
          if(name == "status"){
            if (simple_session.roles()->Is("USER")){
              response.set_body("{\"status\":1}");
            }else{
              response.set_body("{\"status\":0}");
            }
          } else if (name == "login"){

            // log-in the user or not by adding the USER roles
            // to the session.
            bool authenticated = false;

            std::unordered_map<std::string, std::unordered_map<std::string, std::vector<unsigned char>>> parsed_data = granada::http::parser::ParseMultipartFormData(request);

            auto it = parsed_data.find("username");
            if (it != parsed_data.end()){
              std::vector<unsigned char> content = it->second["value"];
              std::string username(content.begin(), content.end());
              auto it2 = parsed_data.find("password");
              if (it2 != parsed_data.end()){
                content = it2->second["value"];
                std::string password(content.begin(), content.end());
                if(username=="user" && password=="pass"){
                  simple_session.roles()->Add("USER");
                  simple_session.roles()->SetProperty("USER","username",username);
                  business::Cart cart(&simple_session);
                  cart.SwitchToUser();
                  authenticated = true;
                  response.set_body("{\"status\":1}");
                }
              }
            }
            if (!authenticated){
              response.set_body("{\"status\":0}");
            }
          } else if (name == "logout"){
            simple_session.roles()->Remove("USER");
            response.set_body("{\"status\":0}");
          }
        }
        response.set_status_code(status_codes::OK);
        response.headers().add(U("Content-Type"), "text/json; charset=utf-8");
        request.reply(response);
      }

    }
  }
}
