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

#include "message_controller.h"

using namespace web::http::details;
using namespace web::http::oauth2::details;

namespace granada{
  namespace http{
    namespace controller{
      MessageController::MessageController(utility::string_t url, std::shared_ptr<granada::http::session::SessionFactory>& session_factory, std::shared_ptr<granada::cache::CacheHandler>& cache)
      {
		  session_factory_ = session_factory;
		  cache_ = cache;
        n_generator_ = std::unique_ptr<utility::nonce_generator>(new utility::nonce_generator(32));
        m_listener_ = std::unique_ptr<http_listener>(new http_listener(url));
        m_listener_->support(methods::PUT, std::bind(&MessageController::handle_put, this, std::placeholders::_1));
        m_listener_->support(methods::POST, std::bind(&MessageController::handle_post, this, std::placeholders::_1));
        m_listener_->support(methods::DEL, std::bind(&MessageController::handle_delete, this, std::placeholders::_1));
      }


      void MessageController::handle_put(web::http::http_request request)
      {
        web::http::http_response response;

        // extract message from HTTP request.
		std::string body = utility::conversions::to_utf8string(request.extract_string().get());
        std::unordered_map<std::string, std::string> parsed_data;
        std::string token;
        std::string message_str = "";
        try{
          // parse the body of the HTTP request and extract the client properties.
          parsed_data = granada::http::parser::ParseQueryString(body);
          token.assign(parsed_data["token"]);
          message_str.assign(parsed_data["message"]);
        }catch(const std::exception e){}

        std::string json_str;

        if (message_str.empty() || token.empty()){
          json_str.assign("{\"error\":\"invalid_request\",\"error_description\":\"The request is missing a valid token or a valid message key.\"}");
        }else{

          // retrieve session if it already exists.
          std::unique_ptr<granada::http::session::Session> session = session_factory_->Session_unique_ptr(token);

          // insert the message if the user has the permission,
          if(session->roles()->Is("msg.insert")){
            granada::Message message(cache_);

            std::string username = session->roles()->GetProperty("msg.insert","username");

            // insert message.
            message.Create(username,message_str);

            // retrieve the actualized list of messages after insertion
            // if the user has the permissions.
            if(session->roles()->Is("msg.select")){
              std::string message_list = message.List(username);
              json_str.assign("{\"description\":\"Success inserting message.\",\"data\":" + message_list + "}");
            }else{
              json_str.assign("{\"description\":\"Success inserting message.\",\"data\":[]}");
            }

          }else{
            json_str.assign("{\"error\":\"access_denied\",\"error_description\":\"Error inserting message. Check if you have the permissions to create messages.\"}");
          }
        }

		web::json::value json = web::json::value::parse(utility::conversions::to_string_t(json_str));
        if (token.empty()){
          response.set_body(json);
          response.set_status_code(status_codes::OK);
          response.headers().add(header_names::content_type, U("text/json; charset=utf-8"));
          request.reply(response);
        }else{
          request.reply(status_codes::OK,json);
        }

      }

      void MessageController::handle_post(web::http::http_request request)
      {

        web::http::http_response response;

        auto paths = uri::split_path(uri::decode(request.relative_uri().path()));

		std::string body = utility::conversions::to_utf8string(request.extract_string().get());
        std::unordered_map<std::string, std::string> parsed_data;
        std::string token;
        try{
          // parse the body of the HTTP request and extract the client properties.
          parsed_data = granada::http::parser::ParseQueryString(body);
          token.assign(parsed_data["token"]);
        }catch(const std::exception e){}

        std::string json_str;

        if (!paths.empty()){

		  std::string name = utility::conversions::to_utf8string(paths[0]);


          // Retrieves session if it exists
          if (token.empty()){
            json_str.assign("{\"error\":\"invalid_token\",\"error_description\":\"The request is missing a valid token.\"}");
          }else{
            // retrieve session if it already exists.
            std::unique_ptr<granada::http::session::Session> session = session_factory_->Session_unique_ptr(token);

            if(name == "list"){

              // retrieve the user's list of messages if the user
              // has the permission.
              if(session->roles()->Is("msg.select")){

                granada::Message message(cache_);

                std::string username = session->roles()->GetProperty("msg.select","username");

                std::string message_list = message.List(username);

                json_str.assign("{\"description\":\"Success listing messages.\",\"data\":" + message_list + "}");
              }else{
                json_str.assign("{\"error\":\"access_denied\",\"error_description\":\"Error listing messages. Check if you have the permissions to read messages.\"}");
              }
            }else if (name == "edit"){

              // Edit messages if user has the permission.

              // extract data from the HTTP request.
			  std::string body = utility::conversions::to_utf8string(request.extract_string().get());
              std::string message_key = "";
              std::string message_str = "";
              try{
                // parse the body of the HTTP request and extract the username, password and scope.
                // scope contains the roles we want to assign to the session.
                message_key.assign(parsed_data["key"]);
                message_str.assign(parsed_data["message"]);
              }catch(const std::exception e){}

              if(session->roles()->Is("msg.update")){
                granada::Message message(cache_);

                std::string username = session->roles()->GetProperty("msg.update","username");

                if (message.Edit(username,message_key,message_str)){
                  if(session->roles()->Is("msg.select")){
                    std::string message_list = message.List(username);
                    json_str.assign("{\"description\":\"Success editing message.\",\"data\":" + message_list + "}");
                  }else{
                    json_str.assign("{\"description\":\"Success editing message.\",\"data\":[]}");
                  }
                }else{
                  json_str.assign("{\"error\":\"invalid_message_key\",\"error_description\":\"Error editing message. The message with given key does not exist.\"}");
                }
              }else{
                json_str.assign("{\"error\":\"access_denied\",\"error_description\":\"Error editing message. Check if you have the permissions to edit messages.\"}");
              }
            }else{
              json_str.assign("{\"error\":\"invalid_request\",\"error_description\":\"The request is missing a required parameter, includes an invalid parameter value, includes a parameter more than once, or is otherwise malformed.\"}");
            }
          }
        }else{
          json_str.assign("{\"error\":\"invalid_request\",\"error_description\":\"The request is missing a required parameter, includes an invalid parameter value, includes a parameter more than once, or is otherwise malformed.\"}");
        }
		web::json::value json = web::json::value::parse(utility::conversions::to_string_t(json_str));
        if (token.empty()){
          response.set_body(json);
          response.set_status_code(status_codes::OK);
          response.headers().add(header_names::content_type, U("text/json; charset=utf-8"));
          request.reply(response);
        }else{
          request.reply(status_codes::OK,json);
        }
      }

      void MessageController::handle_delete(web::http::http_request request)
      {
        web::http::http_response response;

        // extract message from HTTP request.
		std::string body = utility::conversions::to_utf8string(request.extract_string().get());
        std::unordered_map<std::string, std::string> parsed_data;
        std::string token;
        std::string message_key = "";
        try{
          // parse the body of the HTTP request and extract the client properties.
          parsed_data = granada::http::parser::ParseQueryString(body);
          token.assign(parsed_data["token"]);
          message_key.assign(parsed_data["key"]);
        }catch(const std::exception e){}

        std::string json_str;

        if (message_key.empty() || token.empty()){
          json_str.assign("{\"error\":\"invalid_request\",\"error_description\":\"The request is missing a valid token or a valid message key.\"}");
        }else{

          // Retrieves session if it exists
          std::unique_ptr<granada::http::session::Session> session = session_factory_->Session_unique_ptr(token);

          // Delete message if the user has the permission.
          if(session->roles()->Is("msg.delete")){
            std::string username = session->roles()->GetProperty("msg.delete","username");
            granada::Message message(cache_);
            message.Delete(username,message_key);

            // retrieve the actualized list of messages after deletion
            // if the user has the permissions.
            if(session->roles()->Is("msg.select")){
              std::string message_list = message.List(username);
              json_str.assign("{\"description\":\"Success deleting message.\",\"data\":" + message_list + "}");
            }else{
              json_str.assign("{\"description\":\"Success deleting message.\",\"data\":[]}");
            }
          }else{
            json_str.assign("{\"error\":\"access_denied\",\"error_description\":\"Error deleting message. Check if you have the permissions to delete messages.\"}");
          }
        }

		web::json::value json = web::json::value::parse(utility::conversions::to_string_t(json_str));
        if (token.empty()){
          response.set_body(json);
          response.set_status_code(status_codes::OK);
          response.headers().add(header_names::content_type, U("text/json; charset=utf-8"));
          request.reply(response);
        }else{
          request.reply(status_codes::OK,json);
        }
      }


      void MessageController::MessageApplicationSessionFactory(std::unique_ptr<granada::http::session::Session>& session, web::http::http_request request, web::http::http_response response){
        std::unordered_map<std::string, std::string> cookies = granada::http::parser::ParseCookies(request);
        const std::string token_label = "message_token";
        auto it = cookies.find(token_label);
        if (it == cookies.end()){
          session = session_factory_->Session_unique_ptr();
          session->Open();
          response.headers().add(U("Set-Cookie"), utility::conversions::to_string_t(token_label + "=" + session->GetToken() + "; path=/"));
        }else{
          std::string token = it->second;
          session = session_factory_->Session_unique_ptr(token);
          if (session->GetToken().empty() || session->IsGarbage()){
            session->Open();
            response.headers().add(U("Set-Cookie"), utility::conversions::to_string_t(token_label + "=" + session->GetToken() + "; path=/"));
          }
        }
      }
    }
  }
}
