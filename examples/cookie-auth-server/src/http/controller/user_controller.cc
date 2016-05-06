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

#include "user_controller.h"

namespace granada{
  namespace http{
    namespace controller{
      UserController::UserController(utility::string_t url)
      {
        m_listener_ = std::unique_ptr<http_listener>(new http_listener(url));
        m_listener_->support(methods::POST, std::bind(&UserController::handle_post, this, std::placeholders::_1));
      }

      void UserController::handle_post(web::http::http_request request)
      {
        web::http::http_response response;

        std::string body = request.extract_string().get();
        std::string username;
        std::string password;
        try{
          // parse the body of the HTTP request and extract the username and password.
          std::unordered_map<std::string, std::string>  parsed_data =	granada::http::parser::ParseQueryString(body);
          username.assign(parsed_data["username"]);
          password.assign(parsed_data["password"]);
        }catch(const std::exception& e){}

        std::string message = "";

        if (username.empty() || password.empty()){
          response.headers().add(U("Location"), U("/?error=invalid_request&error_description=Error creating user, introduce a username and a password."));
        }else{

          // create user
          granada::User user;
          if (user.Create(username,password)){
            response.headers().add(U("Location"), U("/?description=User " + username + " created successfully."));
          }else{
            response.headers().add(U("Location"), U("/?error=invalid_username&error_description=Error creating user, username already exists."));
          }
        }

        response.set_status_code(status_codes::Found);
        request.reply(response);

      }
    }
  }
}
