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
  * Oauth2 Controller
  *
  */

#include "granada/http/controller/oauth2_controller.h"

using namespace web::http::details;
using namespace web::http::oauth2::details;

namespace granada{
  namespace http{
    namespace controller{

      OAuth2Controller::OAuth2Controller(
        utility::string_t url,
        std::shared_ptr<granada::http::session::SessionCheckpoint>& session_checkpoint,
        std::shared_ptr<granada::http::oauth2::OAuth2Factory>& oauth2_factory)
      {
        m_listener_ = std::unique_ptr<http_listener>(new http_listener(url));
        m_listener_->support(methods::GET, std::bind(&OAuth2Controller::handle_get, this, std::placeholders::_1));
        m_listener_->support(methods::POST, std::bind(&OAuth2Controller::handle_post, this, std::placeholders::_1));
        m_listener_->support(methods::DEL, std::bind(&OAuth2Controller::handle_delete, this, std::placeholders::_1));
        session_checkpoint_ = session_checkpoint;
        oauth2_factory_ = oauth2_factory;
        url_ = url;
        LoadProperties();
      }

      void OAuth2Controller::handle_get(web::http::http_request request){

        web::http::http_response response;

        auto paths = uri::split_path(uri::decode(request.relative_uri().path()));

        int status_code;

        if (paths.empty() || paths.size() != 1){
          status_code = status_codes::Forbidden;
        }else{
          std::string name = paths[0];

          std::string query_string = request.request_uri().query();
          granada::http::oauth2::OAuth2Parameters oauth2_parameters(query_string);

          if (name == oauth2_authorize_uri_){

            // check that the oauth2 patameters are there.
            if (oauth2_parameters.error.empty() && (oauth2_parameters.response_type.empty() || oauth2_parameters.client_id.empty())){
              status_code = status_codes::Forbidden;
            }else{
              std::shared_ptr<granada::http::session::Session> session = session_checkpoint_->check(request,response);

              std::unordered_map<std::string, std::string> values = oauth2_parameters.to_unordered_map();
              values.insert(std::make_pair(entity_keys::oauth2_authorization_form_action, url_ + "/" + oauth2_authorize_uri_));

              // check if user is already associated with a session
              // with the asked roles.
              bool has_all_roles = false;
              if (!oauth2_parameters.scope.empty()){
                has_all_roles = true;
                std::vector<std::string> roles;
                granada::util::string::split(oauth2_parameters.scope, ' ', roles);
                for (auto it = roles.begin(); it != roles.end(); ++it){
                  if (!session->roles()->Is(*it)) {
                    has_all_roles = false;
                  }
                }
              }

              if(has_all_roles){
                // only show website with message
                // with state
                std::string authorizing_message_template = *oauth2_authorizing_message_template_;
                granada::util::string::replace(authorizing_message_template,values);
                response.set_body(authorizing_message_template);
              }else{
                // show message and login.
                // with state
                std::string authorizing_login_template = *oauth2_authorizing_login_template_;
                granada::util::string::replace(authorizing_login_template,values);
                response.set_body(authorizing_login_template);
              }

              status_code = status_codes::OK;
            }

          }else if (name == oauth2_logout_uri_){
            std::shared_ptr<granada::http::session::Session> session = session_checkpoint_->check(request,response);
            session->Close();
            response.set_body(*oauth2_logout_template_);
            status_code = status_codes::OK;
          }else if(name == oauth2_info_uri_){
            // oauth2 response parameters.
            granada::http::oauth2::OAuth2Parameters oauth2_response;

            web::json::value json;
            // only provide information if user is logged
            std::shared_ptr<granada::http::session::Session> authorization_server_session = session_checkpoint_->check(request,response);
            if (authorization_server_session->roles()->Is(entity_keys::oauth2_session_role)){
              oauth2_parameters.username = authorization_server_session->roles()->GetProperty(entity_keys::oauth2_session_role,entity_keys::oauth2_session_role_username);
              std::shared_ptr<granada::http::oauth2::OAuth2Authorization> oauth2_authorization = oauth2_factory_->OAuth2Authorization(oauth2_parameters,session_checkpoint_,oauth2_factory_);
              json = oauth2_authorization->Information();
            }else{
              oauth2_response.error = oauth2_errors::access_denied;
              oauth2_response.error_description = oauth2_errors_description::access_denied;
              json = oauth2_response.to_json();
            }
            response.set_status_code(status_codes::OK);
            response.set_body(json);
            request.reply(response);
            return;
          }else{
            status_code = status_codes::Forbidden;
          }
        }

        if (status_code == status_codes::Forbidden){
          std::deque<std::pair<std::string, std::string>> values;
          values.push_back(std::make_pair(oauth2_errors::error,"403"));
          values.push_back(std::make_pair(oauth2_errors::error_description,"Forbidden"));
          std::string authorizing_error_template = *oauth2_authorizing_error_template_;
          granada::util::string::replace(authorizing_error_template,values);
          response.set_body(authorizing_error_template);
        }

        response.headers().add(header_names::content_type, "text/html; charset=utf-8");
        response.set_status_code(status_code);
        request.reply(response);

      }

      void OAuth2Controller::handle_post(web::http::http_request request)
      {

        web::http::http_response response;
        response.headers().add(header_names_2::access_control_allow_origin, U("*"));

        auto paths = uri::split_path(uri::decode(request.relative_uri().path()));

        // oauth2 response parameters.
        granada::http::oauth2::OAuth2Parameters oauth2_response;

        if (!paths.empty() && paths.size() == 1 && paths[0] == oauth2_authorize_uri_){

          // extract data from the HTTP request.
          std::string body = request.extract_string().get();

          // oauth2 parameters obtained from HTTP request body.
          granada::http::oauth2::OAuth2Parameters oauth2_parameters(body);

          std::shared_ptr<granada::http::oauth2::OAuth2Authorization> oauth2_authorization = oauth2_factory_->OAuth2Authorization(oauth2_parameters,session_checkpoint_,oauth2_factory_);
          oauth2_response = oauth2_authorization->Grant(request,response);

          if (oauth2_parameters.grant_type == oauth2_strings::authorization_code){
            // reply with a json to the client.
            if (oauth2_response.redirect_uri.empty()){
              oauth2_response.redirect_uri = granada::http::parser::ParseURIFromReferer(request);
            }
            response.set_status_code(status_codes::OK);
            response.set_body(oauth2_response.to_json());
            request.reply(response);
            return;
          }
        }else{
          oauth2_response.error = oauth2_errors::invalid_request;
          oauth2_response.error_description = oauth2_errors_description::invalid_request;
        }

        // redirect user with oauth2 response parameters.
        if (oauth2_response.redirect_uri.empty()){
          oauth2_response.redirect_uri = granada::http::parser::ParseURIFromReferer(request);
        }
        response.set_status_code(status_codes::Found);
        std::string redirect_uri = oauth2_response.redirect_uri;
        oauth2_response.redirect_uri.assign("");
        response.headers().add(header_names::location, U(redirect_uri+oauth2_response.to_query_string()));
        request.reply(response);
      }


      void OAuth2Controller::handle_delete(web::http::http_request request){

        web::http::http_response response;

        granada::http::oauth2::OAuth2Parameters oauth2_response;

        std::string query_string = request.request_uri().query();
        granada::http::oauth2::OAuth2Parameters oauth2_parameters(query_string);

        web::json::value json;

        if (oauth2_parameters.client_id.empty()){
          oauth2_response.error = oauth2_errors::invalid_request;
          oauth2_response.error_description = oauth2_errors_description::invalid_request;
          json = oauth2_response.to_json();
        }else{
          // Allow deletion only if user is logged
          std::shared_ptr<granada::http::session::Session> authorization_server_session = session_checkpoint_->check(request,response);
          if (authorization_server_session->roles()->Is(entity_keys::oauth2_session_role)){
            oauth2_parameters.username = authorization_server_session->roles()->GetProperty(entity_keys::oauth2_session_role,entity_keys::oauth2_session_role_username);
            std::shared_ptr<granada::http::oauth2::OAuth2Authorization> oauth2_authorization = oauth2_factory_->OAuth2Authorization(oauth2_parameters,session_checkpoint_,oauth2_factory_);
            json = oauth2_authorization->Delete();
          }
        }

        response.set_status_code(status_codes::OK);
        response.set_body(json);
        request.reply(response);
      }


      void OAuth2Controller::LoadProperties(){

        // sub URI of authorization, example: authorize
        // in that case the full URL for authorization will be my-domain.com/oauth2/authorize
        oauth2_authorize_uri_.assign(granada::util::application::GetProperty(entity_keys::oauth2_authorize_uri));
        if (oauth2_authorize_uri_.empty()){
          oauth2_authorize_uri_.assign(default_strings::oauth2_authorize_uri);
        }

        // sub URI of logout user from authorization server, example: logout
        // in that case the full URL for authorization will be my-domain.com/oauth2/logout
        oauth2_logout_uri_.assign(granada::util::application::GetProperty(entity_keys::oauth2_logout_uri));
        if (oauth2_logout_uri_.empty()){
          oauth2_logout_uri_.assign(default_strings::oauth2_logout_uri);
        }


        // sub URI of info user from authorization server, example: info
        // in that case the full URL for authorization will be my-domain.com/oauth2/info
        oauth2_info_uri_.assign(granada::util::application::GetProperty(entity_keys::oauth2_info_uri));
        if (oauth2_info_uri_.empty()){
          oauth2_info_uri_.assign(default_strings::oauth2_info_uri);
        }

        ////
        // HTML Templates loading.
        //
        // Load the HTML templates for displaying client authorization to the user

        // load the HTML to show in case the user is not already logged in our auth server,
        // we ask the user to enter his/her credentials.
        LoadHTMLTemplate(entity_keys::oauth2_authorizing_login_template, oauth2_templates::oauth2_authorizing_login, oauth2_authorizing_login_template_);

        // load message template, shown in case the user is already logged in our auth server,
        // we don't ask the user it's credentials, instead we show one button for authorizing the
        // client and another for denying authorization.
        LoadHTMLTemplate(entity_keys::oauth2_authorizing_message_template, oauth2_templates::oauth2_authorizing_message, oauth2_authorizing_message_template_);

        // load logout page, this HTML will show if user manually logout from the auth server..
        LoadHTMLTemplate(entity_keys::oauth2_logout_template, oauth2_templates::oauth2_logout, oauth2_logout_template_);

        // load error page, in case, the url is not well formed this HTML will show.
        LoadHTMLTemplate(entity_keys::oauth2_authorizing_error_template, oauth2_templates::oauth2_authorizing_error, oauth2_authorizing_error_template_);
      }


      void OAuth2Controller::LoadHTMLTemplate(const std::string& property_name, const std::string& default_template, std::shared_ptr<std::string>& html_template){
        std::string template_path = granada::util::application::GetProperty(property_name);
        if (html_template.get() == nullptr){
          html_template = std::shared_ptr<std::string>(new std::string());
        }
        if (template_path.empty()){
          html_template->assign(default_template);
        }else{
          if (template_path[0]!='/'){
            template_path = granada::util::application::get_selfpath() + "/" + template_path;
          }
          std::ifstream ifs(template_path.c_str());
          html_template->assign((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
        }
      }


    }
  }
}
