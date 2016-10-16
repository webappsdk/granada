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
      UserController::UserController(utility::string_t url, std::shared_ptr<granada::http::oauth2::OAuth2Factory>& oauth2_factory)
      {
        m_listener_ = std::unique_ptr<http_listener>(new http_listener(url));
        m_listener_->support(methods::POST, std::bind(&UserController::handle_post, this, std::placeholders::_1));
        oauth2_factory_ = oauth2_factory;
      }

      void UserController::handle_post(web::http::http_request request)
      {
        web::http::http_response response;

        std::string body = request.extract_string().get();
        std::string username;
        std::string password;
        std::string password2;
        try{
          // parse the body of the HTTP request and extract the username and password.
          std::unordered_map<std::string, std::string>  parsed_data =	granada::http::parser::ParseQueryString(body);
          username.assign(parsed_data["username"]);
          password.assign(parsed_data["password"]);
          password2.assign(parsed_data["password2"]);
        }catch(const std::exception& e){}

        std::string message = "";

        if (username.empty() || password.empty() || password2.empty()){
          response.headers().add(U("Location"), U("?error=invalid_request&error_description=Error creating user, introduce a username and a password."));
        }else{
          if (password==password2){
            if (username.length() > 5 && username.length() < 16 && password.length() > 5 && password.length() < 16){
              // create user
              std::unique_ptr<granada::http::oauth2::OAuth2User> oauth2_user = oauth2_factory_->OAuth2User_unique_ptr();
              web::json::value roles;
              std::string roles_str = "{\"msg.select\":{\"username\":\"{{username}}\"},\"msg.insert\":{\"username\":\"{{username}}\"},\"msg.update\":{\"username\":\"{{username}}\"},\"msg.delete\":{\"username\":\"{{username}}\"}}";
              std::deque<std::pair<std::string,std::string>> values;
              values.push_back(std::make_pair("username",username));
              granada::util::string::replace(roles_str,values);
              try{
                roles = web::json::value::parse(roles_str);
              }catch(const web::json::json_exception& e){
                roles_str = "{}";
                roles = web::json::value::parse(roles_str);
              }
              if (oauth2_user->Create(username,password,roles)){
                response.headers().add(U("Location"), U("/?description=User " + username + " created successfully."));
              }else{
                response.headers().add(U("Location"), U("/?error=invalid_username&error_description=Error creating user, username already exists."));
              }
            }else{
              response.headers().add(U("Location"), U("/?error=invalid_field&error_description=Error creating user, username and password lengths have to be at least 6 characters and less than 16."));
            }
          }else{
            response.headers().add(U("Location"), U("/?error=invalid_request&error_description=Error creating user, the two passwords introduced were different."));
          }
        }

        response.set_status_code(status_codes::Found);
        request.reply(response);

      }


    }
  }
}
