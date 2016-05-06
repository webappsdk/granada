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

namespace granada{
  namespace http{
    namespace controller{
      MessageController::MessageController(utility::string_t url)
      {
        m_listener_ = std::unique_ptr<http_listener>(new http_listener(url));
        m_listener_->support(methods::PUT, std::bind(&MessageController::handle_put, this, std::placeholders::_1));
        m_listener_->support(methods::POST, std::bind(&MessageController::handle_post, this, std::placeholders::_1));
        m_listener_->support(methods::DEL, std::bind(&MessageController::handle_delete, this, std::placeholders::_1));
      }

      void MessageController::handle_put(web::http::http_request request)
      {
        web::http::http_response response;

        // extract message from HTTP request.
        std::string message_str = "";
        const web::json::value& obj = request.extract_json().get();
        if (obj.is_object() && obj.has_field("message")){
          const web::json::value message_json = obj.at("message");
          if (message_json.is_string()){
            message_str = message_json.as_string();
          }
        }

        if (message_str.empty()){
          response.set_body("{\"error\":\"invalid_request\",\"error_description\":\"The request is missing a required parameter, includes an invalid parameter value, includes a parameter more than once, or is otherwise malformed.\"}");
        }else{

          // Retrieves session if it exists
          granada::http::session::RedisStorageSession redis_storage_session(request,response);

          // insert the message if the user has the permission,
          if(redis_storage_session.roles()->Is("MSG_WRITER")){
            granada::Message message;

            std::string username = redis_storage_session.roles()->GetProperty("MSG_WRITER","username");

            // insert message.
            message.Create(username,message_str);

            // retrieve the actualized list of messages after insertion
            // if the user has the permissions.
            if(redis_storage_session.roles()->Is("MSG_READER")){
              std::string message_list = message.List(username);
              response.set_body("{\"description\":\"Success inserting message.\",\"data\":" + message_list + "}");
            }else{
              response.set_body("{\"description\":\"Success inserting message.\",\"data\":[]}");
            }

          }else{
            response.set_body("{\"error\":\"unauthorized_client\",\"error_description\":\"Error inserting message. Check if you have the permissions to write messages.\"}");
          }
        }

        response.set_status_code(status_codes::OK);
        response.headers().add(U("Content-Type"), "text/json; charset=utf-8");

        request.reply(response);

      }

      void MessageController::handle_post(web::http::http_request request)
      {

        web::http::http_response response;

        auto paths = uri::split_path(uri::decode(request.relative_uri().path()));

        if (!paths.empty()){
          std::string name = paths[0];

          if(name == "list"){

            // Retrieves session if it exists
            granada::http::session::RedisStorageSession redis_storage_session(request, response);

            // retrieve the user's list of messages if the user
            // has the permission.
            if(redis_storage_session.roles()->Is("MSG_READER")){

              granada::Message message;

              std::string username = redis_storage_session.roles()->GetProperty("MSG_READER","username");

              std::string message_list = message.List(username);

              response.set_body("{\"description\":\"Success listing messages.\",\"data\":" + message_list + "}");
            }else{
              response.set_body("{\"error\":\"unauthorized_client\",\"error_description\":\"Error listing messages. Check if you have the permissions to read messages.\"}");
            }
          }else if (name == "edit"){

            // Edit messages if user has the permission.

            // extract the message key and the message text from the HTTP request.
            std::string message_key = "";
            std::string message_str = "";
            const web::json::value& obj = request.extract_json().get();
            if (obj.is_object() && obj.has_field("message") && obj.has_field("key")){
              const web::json::value message_json = obj.at("message");
              if (message_json.is_string()){
                message_str = message_json.as_string();
              }
              const web::json::value key_json = obj.at("key");
              if (key_json.is_string()){
                message_key = key_json.as_string();
              }
            }

            // Retrieves session if it exists
            granada::http::session::RedisStorageSession redis_storage_session(request,response);

            if(redis_storage_session.roles()->Is("MSG_WRITER")){
              granada::Message message;

              std::string username = redis_storage_session.roles()->GetProperty("MSG_READER","username");

              if (message.Edit(username,message_key,message_str)){
                if(redis_storage_session.roles()->Is("MSG_READER")){
                  std::string message_list = message.List(username);
                  response.set_body("{\"description\":\"Success editing message.\",\"data\":" + message_list + "}");
                }else{
                  response.set_body("{\"description\":\"Success editing message.\",\"data\":[]}");
                }
              }else{
                response.set_body("{\"error\":\"invalid_message\",\"error_description\":\"Error editing message. The message does not exist.\"}");
              }
            }else{
              response.set_body("{\"error\":\"unauthorized_client\",\"error_description\":\"Error editing message. Check if you have the permissions to write messages.\"}");
            }
          }else{
            response.set_body("{\"error\":\"invalid_request\",\"error_description\":\"The request is missing a required parameter, includes an invalid parameter value, includes a parameter more than once, or is otherwise malformed.\"}");
          }
        }else{
          response.set_body("{\"error\":\"invalid_request\",\"error_description\":\"The request is missing a required parameter, includes an invalid parameter value, includes a parameter more than once, or is otherwise malformed.\"}");
        }

        response.set_status_code(status_codes::OK);
        response.headers().add(U("Content-Type"), "text/json; charset=utf-8");
        request.reply(response);
      }

      void MessageController::handle_delete(web::http::http_request request)
      {
        web::http::http_response response;

        // Extract message key from HTTP request.
        std::string message_key = "";
        const web::json::value& obj = request.extract_json().get();
        if (obj.is_object() && obj.has_field("key")){
          const web::json::value key_json = obj.at("key");
          if (key_json.is_string()){
            message_key = key_json.as_string();
          }
        }

        // Retrieves session if it exists
        granada::http::session::RedisStorageSession redis_storage_session(request,response);

        // Delete message if the user has the permission.
        if(redis_storage_session.roles()->Is("MSG_WRITER")){
          std::string username = redis_storage_session.roles()->GetProperty("MSG_WRITER","username");
          granada::Message message;
          message.Delete(username,message_key);

          // retrieve the actualized list of messages after deletion
          // if the user has the permissions.
          if(redis_storage_session.roles()->Is("MSG_READER")){
            std::string message_list = message.List(username);
            response.set_body("{\"description\":\"Success deleting message.\",\"data\":" + message_list + "}");
          }else{
            response.set_body("{\"description\":\"Success deleting message.\",\"data\":[]}");
          }
        }else{
          response.set_body("{\"error\":\"unauthorized_client\",\"error_description\":\"Error deleting message. Check if you have the permissions to write messages.\"}");
        }

        response.set_status_code(status_codes::OK);
        response.headers().add(U("Content-Type"), "text/json; charset=utf-8");
        request.reply(response);
      }
    }
  }
}
