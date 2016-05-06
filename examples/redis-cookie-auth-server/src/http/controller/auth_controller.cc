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

        std::string uri;

        web::http::http_response response;

        // extract data from the HTTP request.
        std::string body = request.extract_string().get();

        std::string username;
        std::string password;
        std::string scope;

        try{
          // parse the body of the HTTP request and extract the username, password and scope.
          // scope contains the roles we want to assign to the session.
          std::unordered_map<std::string, std::string>  parsed_data =	granada::http::parser::ParseQueryString(body);
          username.assign(parsed_data["username"]);
          password.assign(parsed_data["password"]);
          scope.assign(parsed_data["scope"]);
        }catch(const std::exception& e){}

        if (!username.empty() && !password.empty() && !scope.empty()){

          granada::http::session::RedisStorageSession redis_storage_session(request,response);

          granada::User user;

          // if the user credentials are OK, opens a session and assign
          // the asked roles.
          if (user.AssignSession(username,password,&redis_storage_session,response)){

            // check if the user's session can have the asked roles
            // or if it is not allowed to have them.
            std::string user_roles = user.Roles(username);
            bool has_client_role = false;

            bool role_assigned = false;
            std::vector<std::string> roles;
            granada::util::string::split(scope, '+', roles);
            int len = roles.size();
            std::string role;
            while (len--){
              role.assign(roles[len]);

              // check if role is included in user roles
              if (user_roles.find(role) != std::string::npos) {
                has_client_role = true;
                if (role == "MSG_READER"){
                  redis_storage_session.roles()->Add("MSG_READER");
                  redis_storage_session.roles()->SetProperty("MSG_READER", "username", username);
                  role_assigned = true;
                }else if (role == "MSG_WRITER"){
                  redis_storage_session.roles()->Add("MSG_WRITER");
                  redis_storage_session.roles()->SetProperty("MSG_WRITER", "username", username);
                  role_assigned = true;
                }else if (role == "USER"){
                  redis_storage_session.roles()->Add("USER");
                  redis_storage_session.roles()->SetProperty("USER", "username", username);
                  role_assigned = true;
                }
              }
            }

            if (has_client_role){
              if (role_assigned) {
                uri.assign("/messenger/list?access_token=cookie&token_type=bearer&scope=" + scope + "&description=Token with " + scope + " roles obtained.");
              }else{
                uri.assign("/?error=invalid_scope&error_description=The requested scope is invalid, unknown, or malformed.");
              }
            }else{
              uri.assign("/?error=unauthorized_client&error_description=The authenticated client is not authorized to use this authorization grant type.");
            }
          }else{
            uri.assign("/?error=invalid_client&error_description=Client authentication failed.");
          }
        }else{
          uri.assign("/?error=invalid_request&error_description=The request is missing a required parameter, includes an invalid parameter value, includes a parameter more than once, or is otherwise malformed.");
        }

        response.set_status_code(status_codes::Found);
        response.headers().add(U("Location"), U(uri));
        request.reply(response);
      }
    }
  }
}
