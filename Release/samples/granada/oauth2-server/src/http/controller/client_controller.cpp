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
  * Client Controller
  * Used for registering clients.
  *
  */

#include "client_controller.h"

namespace granada{
  namespace http{
    namespace controller{
      ClientController::ClientController(utility::string_t url, std::shared_ptr<granada::http::oauth2::OAuth2Factory>& oauth2_factory)
      {
        m_listener_ = std::unique_ptr<http_listener>(new http_listener(url));
        m_listener_->support(methods::POST, std::bind(&ClientController::handle_post, this, std::placeholders::_1));
        oauth2_factory_ = oauth2_factory;
      }

      void ClientController::handle_post(web::http::http_request request)
      {

        std::string body = request.extract_string().get();
        std::string redirect_uris_str;
        std::string application_name;
        std::string roles_str;


        try{
          // parse the body of the HTTP request and extract the client properties.
          std::unordered_map<std::string, std::string> parsed_data = granada::http::parser::ParseQueryString(body);
          redirect_uris_str.assign(parsed_data["redirect_uri"]);
          application_name.assign(parsed_data["application_name"]);
          roles_str.assign(parsed_data["roles"]);
        }catch(const std::exception& e){}
        web::json::value json;
        if (redirect_uris_str.empty() || application_name.empty() || roles_str.empty()){
          json = web::json::value::parse("{\"error\":\"invalid_request\",\"error_description\":\"Error registering client, a parameter has not been filled.\"}");
        }else{
          // create client
          std::shared_ptr<granada::http::oauth2::OAuth2Client> oauth2_client = oauth2_factory_->OAuth2Client();
          try{
            std::deque<std::pair<std::string,std::string>> values;
            values.push_back(std::make_pair("+", " "));
            granada::util::string::replace(redirect_uris_str,values,"","");
            granada::util::string::replace(roles_str,values,"","");

            std::vector<std::string> redirect_uris;
            granada::util::string::split(redirect_uris_str,' ',redirect_uris);
            std::vector<std::string> roles;
            granada::util::string::split(roles_str,' ',roles);

            // generate client secret
            granada::crypto::CPPRESTNonceGenerator n_generator;
            int password_length = 12;
            std::string password = n_generator.generate(password_length);

            // type public for third party applications
            std::string type = "public";

            oauth2_client->Create(type, redirect_uris, application_name, roles, password);
            json = web::json::value::parse("{\"client_id\":\"" + oauth2_client->GetId() + "\",\"client_secret\":\"" + password + "\",\"description\":\"Client created successfully.\"}");
          }catch(const std::exception& e){
            json = web::json::value::parse("{\"error\":\"server_error\",\"error_description\":\"Error creating client.\"}");
          }
        }

        request.reply(status_codes::OK, json);
      }
    }
  }
}
