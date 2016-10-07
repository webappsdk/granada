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

#include "application_controller.h"

using namespace web::http::details;
using namespace web::http::oauth2::details;

namespace granada{
  namespace http{
    namespace controller{
      ApplicationController::ApplicationController(utility::string_t url,std::shared_ptr<granada::http::session::SessionCheckpoint>& session_checkpoint)
      {
        n_generator_ = std::unique_ptr<utility::nonce_generator>(new utility::nonce_generator(32));
        m_listener_ = std::unique_ptr<http_listener>(new http_listener(url));
        m_listener_->support(methods::GET, std::bind(&ApplicationController::handle_get, this, std::placeholders::_1));
        m_listener_->support(methods::PUT, std::bind(&ApplicationController::handle_put, this, std::placeholders::_1));
        m_listener_->support(methods::POST, std::bind(&ApplicationController::handle_post, this, std::placeholders::_1));
        m_listener_->support(methods::DEL, std::bind(&ApplicationController::handle_delete, this, std::placeholders::_1));

        readcode_client_id_ = std::shared_ptr<std::string>(new std::string());
        readwritecode_client_id_ = std::shared_ptr<std::string>(new std::string());
        readcode_client_secret_ = std::shared_ptr<std::string>(new std::string());
        readwritecode_client_secret_ = std::shared_ptr<std::string>(new std::string());

        readcode_client_id_->assign(GetClientId("readcode"));
        readwritecode_client_id_->assign(GetClientId("readwritecode"));

        session_checkpoint_ = session_checkpoint;
      }


      void ApplicationController::handle_get(web::http::http_request request){

        web::http::http_response response;

        response.set_status_code(status_codes::OK);
        response.headers().add(header_names::content_type, "text/html; charset=utf-8");

        auto paths = uri::split_path(uri::decode(request.relative_uri().path()));

        if (paths.empty()){
          response.set_body("404");
        }else{
          std::string name = paths[0];

          if (name == "readcode" || name == "readwritecode"){
            std::string sub_name;
            if ( paths.size() > 1 ){
              sub_name = paths[1];
            }
            if (sub_name == "auth"){
              granada::http::oauth2::OAuth2Parameters oauth2_response;
              // Retrieves session if it exists
              std::shared_ptr<granada::http::session::Session> session;
              // as the example is all done in localhost I retrieve the session token manually
              MessageApplicationSessionCheckpoint(session, request, response);

              // generate state
              std::string state;
              state.assign(n_generator_->generate());
              session->roles()->SetProperty("msg.user", "state", state);
              session->roles()->SetProperty("msg.user", "state.creation.time", granada::util::time::stringify(std::time(nullptr)));
              oauth2_response.response_type = "code";
              oauth2_response.state = state;
              oauth2_response.client_id = GetClientId(name);
              oauth2_response.scope = GetRoles(name);
              oauth2_response.redirect_uri = "http://localhost/application/" + name;
              std::string redirect_uri = "http://localhost/oauth2/auth";
              response.set_status_code(status_codes::Found);
              response.headers().add(header_names::location, U(redirect_uri+oauth2_response.to_query_string()));
              request.reply(response);

            }else{
              // Retrieves session if it exists
              std::shared_ptr<granada::http::session::Session> session;
              // as the example is all done in localhost I retrieve the session token manually
              MessageApplicationSessionCheckpoint(session, request, response);

              // check if we have recieved a code and a state to obtain access tokens
              std::string query_string = request.request_uri().query();
              std::string code;
              std::string state;
              try{
                // parse the body of the HTTP request and extract the code and the state.
                std::unordered_map<std::string, std::string>  parsed_data =	granada::http::parser::ParseQueryString(query_string);
                code.assign(parsed_data[oauth2_strings::code]);
                state.assign(parsed_data[oauth2_strings::state]);
              }catch(const std::exception& e){}

              bool code_or_state_error = false;

              if (!code.empty() || !state.empty()){
                // check that the state is equal to the state stored in the session
                // This is done to avoid cross-site request forgery.
                std::string msg_user_state = session->roles()->GetProperty("msg.user", "state");
                std::string state_creation_time_str = session->roles()->GetProperty("msg.user", "state.creation.time");
                std::time_t state_creation_time = granada::util::time::parse(state_creation_time_str);

                if (!state.empty() && state == msg_user_state && !granada::util::time::is_timedout(state_creation_time,60)){
                  // process code
                  if (code.empty()){
                    code_or_state_error = true;
                  }else{
                    session->roles()->SetProperty("msg.user", "code", code);
                  }
                }else{
                  // state is not valid show error
                  code_or_state_error = true;
                }

                // remove the stored state
                session->roles()->DestroyProperty("msg.user", "state");
                session->roles()->DestroyProperty("msg.user", "state.creation.time");
              }

              session->roles()->Add("msg.user");
              session->roles()->SetProperty("msg.user", "application", name);

              std::string file_path = granada::util::application::get_selfpath() + "/www/messenger/" + name + "/index.html";
              std::string content(std::istreambuf_iterator<char>(std::ifstream(file_path).rdbuf()),std::istreambuf_iterator<char>());
              response.set_body(content);

            }
          }else{
            response.set_body("404");
          }
        }



        request.reply(response);
      }

      void ApplicationController::handle_put(web::http::http_request request)
      {
        web::http::http_response response;

        // Retrieves session if it exists
        std::shared_ptr<granada::http::session::Session> session;
        // as the example is all done in localhost I retrieve the session token manually
        MessageApplicationSessionCheckpoint(session, request, response);

        // retrieve the user's list of messages if the user
        // has the permission.
        if(session->roles()->Is("msg.user")){

          // get the token.
          std::string access_token = session->roles()->GetProperty("msg.user", "access_token");

          if (access_token.empty()){
            // get access token with the code
            access_token.assign(GetAccessToken(session->roles()->GetProperty("msg.user", "application"), session));
          }

          if (access_token.empty()){
            response.set_body("{\"error\":\"unauthorized_client\",\"error_description\":\"Error creating message. Check if you have the permissions to write messages.\"}");
          }else{
            // resource server request
            std::string error;
            std::string error_description;
            std::string message_list = ResourceServerRequest(request,methods::PUT,access_token,"http://localhost:80/message",error,error_description);
            if (error.empty()){
              response.set_body("{\"description\":\"Success creating message.\",\"data\":" + message_list + "}");
            }else{
              response.set_body("{\"error\":\"" + error + "\",\"error_description\":\"" + error_description + "\"}");
            }
          }

        }else{
          response.set_body("{\"error\":\"unauthorized_client\",\"error_description\":\"Error creating message. Check if you have the permissions to write messages.\"}");
        }

        response.set_status_code(status_codes::OK);
        response.headers().add(header_names::content_type, "text/json; charset=utf-8");

        request.reply(response);

      }

      void ApplicationController::handle_post(web::http::http_request request)
      {

        web::http::http_response response;

        auto paths = uri::split_path(uri::decode(request.relative_uri().path()));

        if (!paths.empty()){
          std::string name = paths[0];

          if (name == "list" || name == "edit"){

            // Retrieves session if it exists
            std::shared_ptr<granada::http::session::Session> session;
            // as the example is all done in localhost I retrieve the session token manually
            MessageApplicationSessionCheckpoint(session, request, response);

            // retrieve the user's list of messages if the user
            // has the permission.
            if(session->roles()->Is("msg.user")){

              // get the token.
              std::string access_token = session->roles()->GetProperty("msg.user", "access_token");

              if (access_token.empty()){
                // get access token with the code
                access_token = GetAccessToken(session->roles()->GetProperty("msg.user", "application"), session);
              }

              if(name == "list"){
                if (access_token.empty()){
                  response.set_body("{\"error\":\"unauthorized_client\",\"error_description\":\"Error listing messages. Check if you have the permissions to read messages.\"}");
                }else{
                  // resource server request
                  std::string error;
                  std::string error_description;
                  std::string message_list = ResourceServerRequest(request,methods::POST,access_token,"http://localhost:80/message/list",error,error_description);
                  if (error.empty()){
                    response.set_body("{\"description\":\"Success listing messages.\",\"data\":" + message_list + "}");
                  }else{
                    response.set_body("{\"error\":\"" + error + "\",\"error_description\":\"" + error_description + "\"}");
                  }
                }
              }else if (name == "edit"){
                if (access_token.empty()){
                  response.set_body("{\"error\":\"unauthorized_client\",\"error_description\":\"Error editing message. Check if you have the permissions to edit messages.\"}");
                }else{
                  // resource server request
                  std::string error;
                  std::string error_description;
                  std::string message_list = ResourceServerRequest(request,methods::POST,access_token,"http://localhost:80/message/edit",error,error_description);
                  if (error.empty()){
                    response.set_body("{\"description\":\"Success editing message.\",\"data\":" + message_list + "}");
                  }else{
                    response.set_body("{\"error\":\"" + error + "\",\"error_description\":\"" + error_description + "\"}");
                  }
                }
              }else{
                response.set_body("{\"error\":\"invalid_request\",\"error_description\":\"The request is missing a required parameter, includes an invalid parameter value, includes a parameter more than once, or is otherwise malformed.\"}");
              }
            }else{
              response.set_body("{\"error\":\"unauthorized_client\",\"error_description\":\"Error listing messages. Check if you have the permissions to read messages.\"}");
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

      void ApplicationController::handle_delete(web::http::http_request request)
      {
        web::http::http_response response;

        // Retrieves session if it exists
        std::shared_ptr<granada::http::session::Session> session;
        // as the example is all done in localhost I retrieve the session token manually
        MessageApplicationSessionCheckpoint(session, request, response);

        // retrieve the user's list of messages if the user
        // has the permission.
        if(session->roles()->Is("msg.user")){

          // get the token.
          std::string access_token = session->roles()->GetProperty("msg.user", "access_token");

          if (access_token.empty()){
            // get access token with the code
            access_token.assign(GetAccessToken(session->roles()->GetProperty("msg.user", "application"), session));
          }

          if (access_token.empty()){
            response.set_body("{\"error\":\"unauthorized_client\",\"error_description\":\"Error deleting message. Check if you have the permissions to write messages.\"}");
          }else{
            // resource server request
            std::string error;
            std::string error_description;
            std::string message_list = ResourceServerRequest(request,methods::DEL,access_token,"http://localhost:80/message",error,error_description);
            if (error.empty()){
              response.set_body("{\"description\":\"Success deleting message.\",\"data\":" + message_list + "}");
            }else{
              response.set_body("{\"error\":\"" + error + "\",\"error_description\":\"" + error_description + "\"}");
            }
          }
        }else{
          response.set_body("{\"error\":\"unauthorized_client\",\"error_description\":\"Error deleting messages. Check if you have the permissions to write messages.\"}");
        }

        response.set_status_code(status_codes::OK);
        response.headers().add(U("Content-Type"), "text/json; charset=utf-8");
        request.reply(response);
      }


      void ApplicationController::MessageApplicationSessionCheckpoint(std::shared_ptr<granada::http::session::Session>& session, web::http::http_request request, web::http::http_response response){
        std::unordered_map<std::string, std::string> cookies = granada::http::parser::ParseCookies(request);
        const std::string token_label = "message_token";
        auto it = cookies.find(token_label);
        if (it == cookies.end()){
          session = session_checkpoint_->check();
          session->Open();
          response.headers().add(U("Set-Cookie"), token_label + "=" + session->GetToken() + "; path=/");
        }else{
          std::string token = it->second;
          session = session_checkpoint_->check(token);
          if (session->GetToken().empty() || session->IsGarbage()){
            session->Open();
            response.headers().add(U("Set-Cookie"), token_label + "=" + session->GetToken() + "; path=/");
          }
        }
      }

      std::string ApplicationController::GetClientId(const std::string& name){
        if (name == "readcode"){
          if (!readcode_client_id_->empty()){
            return *readcode_client_id_;
          }
        }else if (name == "readwritecode"){
          if (!readwritecode_client_id_->empty()){
            return *readwritecode_client_id_;
          }
        }
        // register the client
        // this shouldn't be done here, it is done here just for example
        // purpose,
        //
        std::shared_ptr<std::string> client_id = std::shared_ptr<std::string>(new std::string());
        std::shared_ptr<std::string> client_secret = std::shared_ptr<std::string>(new std::string(""));

        web::uri uri("http://localhost:80/client");
        web::http::client::http_client client(uri);

        web::http::http_request request2(methods::POST);
        request2.set_request_uri(uri);
        request2.set_body(U("redirect_uri=http://localhost/application/" + name + "&application_name=" + GetApplicationName(name) + "&roles=" + GetRoles(name)));

        client.request(request2).then([client_id,client_secret](web::http::http_response response2)
        {
          try{
            web::json::value json = response2.extract_json().get();
            if (json.has_field("client_id")){
              web::json::value client_id_json = json.at("client_id");
              if (client_id_json.is_string()){
                client_id->assign(client_id_json.as_string());
              }
            }
            if (json.has_field("client_secret")){
              web::json::value client_secret_json = json.at("client_secret");
              if (client_secret_json.is_string()){
                client_secret->assign(client_secret_json.as_string());
              }
            }
          }catch(const std::exception& e){
            client_id->assign("0");
          }
        }).wait();

        if (name == "readcode"){
          readcode_client_secret_->assign(*client_secret);
        }else if (name == "readwritecode"){
          readwritecode_client_secret_->assign(*client_secret);
        }

        return *client_id;
      }


      std::string ApplicationController::GetAccessToken(const std::string& name, std::shared_ptr<granada::http::session::Session>& session){
        // register the client
        // this shouldn't be done here, it is done here just for example
        // purpose,

        // retrieve code
        std::string code = session->roles()->GetProperty("msg.user","code");

        std::shared_ptr<std::string> access_token = std::shared_ptr<std::string>(new std::string());

        if (!code.empty()){


          web::uri_builder uri_builder("http://localhost:80/oauth2");
          uri_builder.append_path(U("auth"));
          web::uri uri = uri_builder.to_uri();
          web::http::client::http_client client(uri);

          web::http::http_request request2(methods::POST);
          request2.set_body(U("grant_type=authorization_code&code=" + code + "&redirect_uri=http://localhost/application/" + name + "&client_id=" + GetClientId(name) + "&client_secret=" + GetClientSecret(name)));

          client.request(request2).then([access_token](web::http::http_response response2){
            try{
              web::json::value json = response2.extract_json().get();
              if (json.has_field("access_token")){
                web::json::value access_token_json = json.at("access_token");
                if (access_token_json.is_string()){
                  access_token->assign(access_token_json.as_string());
                }
              }else{
                access_token->assign("");
              }
            }catch(const std::exception& e){
              access_token->assign("");
            }
          }).wait();


        }else{
          access_token->assign("");
        }

        return *access_token;
      }

      std::string ApplicationController::GetRoles(const std::string& name){
        return name == "readwritecode" ? "msg.select msg.insert msg.update msg.delete" : "msg.select";
      }

      std::string ApplicationController::GetApplicationName(const std::string& name){
        return name == "readwritecode" ? "Message_Editor_Application_Code_Grant" : "Message_Reader_Application_Code_Grant";
      }

      std::string ApplicationController::GetClientSecret(const std::string& name){
        return name == "readwritecode" ? *readwritecode_client_secret_ : *readcode_client_secret_;
      }


      std::string ApplicationController::ResourceServerRequest(web::http::http_request& request,
                                                               const std::string& method,
                                                               const std::string& access_token,
                                                               const std::string& address,
                                                               std::string& error,
                                                               std::string& error_description){


        web::uri_builder uri_builder(address);
        web::uri uri = uri_builder.to_uri();
        web::http::client::http_client client(uri);
        std::shared_ptr<std::string> message_list = std::shared_ptr<std::string>(new std::string());
        std::shared_ptr<std::string> error_ptr = std::shared_ptr<std::string>(new std::string());
        std::shared_ptr<std::string> error_description_ptr = std::shared_ptr<std::string>(new std::string());

        std::string body = request.extract_string().get();
        request.set_body(body + "&token=" + access_token);
        request.set_method(method);

        client.request(request).then([message_list,error_ptr,error_description_ptr](web::http::http_response response2){
          try{
            web::json::value json = response2.extract_json().get();
            if (json.has_field("data")){
              web::json::value data_json = json.at("data");
              if (data_json.is_array()){
                message_list->assign(data_json.serialize());
              }
            }else{
              if (json.has_field("error")){
                web::json::value error_json = json.at("error");
                if (error_json.is_string()){
                  error_ptr->assign(error_json.as_string());
                }
              }
              if (json.has_field("error_description")){
                web::json::value error_description_json = json.at("error_description");
                if (error_description_json.is_string()){
                  error_description_ptr->assign(error_description_json.as_string());
                }
              }
            }
          }catch(const std::exception& e){
            message_list->assign("[]");
          }
        }).wait();

        error.assign(*error_ptr);
        error_description.assign(*error_description_ptr);

        return *message_list;
      }

    }
  }
}
