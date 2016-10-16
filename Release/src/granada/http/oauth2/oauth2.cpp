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
  */

#include "granada/http/oauth2/oauth2.h"

#define _OAUTH2_ERRORS
#define DAT(a_, b_) const oauth2_error oauth2_errors::a_(_XPLATSTR(b_));
#include "granada/http/http_constants.dat"
#undef _OAUTH2_ERRORS
#undef DAT

#define _OAUTH2_ERRORS_DESCRIPTION
#define DAT(a_, b_) const oauth2_error_description oauth2_errors_description::a_(_XPLATSTR(b_));
#include "granada/http/http_constants.dat"
#undef _OAUTH2_ERRORS_DESCRIPTION
#undef DAT

#define _OAUTH2_CLIENT_TYPES
#define DAT(a_, b_) const oauth2_client_type oauth2_client_types::a_(_XPLATSTR(b_));
#include "granada/http/http_constants.dat"
#undef _OAUTH2_CLIENT_TYPES
#undef DAT

#define _OAUTH2_STRINGS_2
#define DAT(a_, b_) const oauth2_string_2 oauth2_strings_2::a_(_XPLATSTR(b_));
#include "granada/http/http_constants.dat"
#undef _OAUTH2_STRINGS_2
#undef DAT

#define _TEMPLATES
#define TEMPLATES(a_, b_) const oauth2_template oauth2_templates::a_(_XPLATSTR(b_));
#include "granada/http/oauth2/oauth2.templates"
#undef _TEMPLATES
#undef TEMPLATES


namespace granada{
  namespace http{
    namespace oauth2{

      //////////////////////////////////////////////////
      // OAuth2 Client
      ////

      std::mutex OAuth2Client::oauth2_client_creation_mtx_;
      std::string OAuth2Client::cache_namespace_;
      int OAuth2Client::client_id_length_;

      void OAuth2Client::Load(){

        if (!id_.empty() && Exists()){

          const std::string& hash(this->hash());
          
          // load client properties.
          key_.assign(cache()->Read(hash, entity_keys::oauth2_client_key));
          type_.assign(cache()->Read(hash, entity_keys::oauth2_client_client_type));
          application_name_.assign(cache()->Read(hash,entity_keys::oauth2_client_application_name));

          const std::string& redirect_uris_str(cache()->Read(hash, entity_keys::oauth2_client_redirect_uris));
          granada::util::string::split(redirect_uris_str, ',', redirect_uris_);
          
          const std::string& roles_str(cache()->Read(hash, entity_keys::oauth2_client_roles));
          granada::util::string::split(roles_str, ',', roles_);

          const std::string& creation_time_str(cache()->Read(hash, entity_keys::oauth2_client_creation_time));
          creation_time_ = granada::util::time::parse(creation_time_str);

        }else{
          id_.assign("");
        }
      }


      void OAuth2Client::Load(const std::string& identifier){
        if (!identifier.empty()){
          id_.assign(identifier);
          Load();
        }
      }


      void OAuth2Client::Create(const std::string& type, const std::vector<std::string>& redirect_uris, const std::string& application_name, const std::vector<std::string>& roles, std::string& secret){
        
        id_.assign(nonce_generator()->generate(client_id_length_));

        const std::string& hash(this->hash());

        // save with unique id,
        // check if it does not already exist one client with the same id.
        oauth2_client_creation_mtx_.lock();

        if (Exists()){

          oauth2_client_creation_mtx_.unlock();
          Create(type,redirect_uris,application_name,roles,secret);
        }else{

          // client with that id does not already exist,
          // save it.
          cache()->Write(hash, entity_keys::oauth2_client_id, id_);

          oauth2_client_creation_mtx_.unlock();

          // save the client's properties.
          key_.assign(cryptograph()->Encrypt(id_,secret));
          type_.assign(type);
          redirect_uris_ = redirect_uris;
          roles_ = roles;
          application_name_ = application_name;

          cache()->Write(hash, entity_keys::oauth2_client_key, key_);
          cache()->Write(hash, entity_keys::oauth2_client_client_type, type_);
          cache()->Write(hash, entity_keys::oauth2_client_application_name, application_name_);
          cache()->Write(hash, entity_keys::oauth2_client_redirect_uris, granada::util::vector::stringify(redirect_uris,","));
          cache()->Write(hash, entity_keys::oauth2_client_roles, granada::util::vector::stringify(roles,","));
          cache()->Write(hash, entity_keys::oauth2_client_creation_time, granada::util::time::stringify(std::time(nullptr)));

        }
      }


      bool OAuth2Client::Delete(const std::string& secret){

        if (cryptograph()->Decrypt(key_,secret) == id_){
          cache()->Destroy(hash());
          return true;
        }
        return false;
      }


      bool OAuth2Client::CorrectCredentials(std::string secret){

        std::string decrypted_key = cryptograph()->Decrypt(key_,secret);
        if (decrypted_key.length()>id_.length()){
          decrypted_key.erase(decrypted_key.begin()+id_.length(),decrypted_key.end());
        }
        if (decrypted_key == id_){
          return true;
        }
        return false;
      }


      void OAuth2Client::LoadProperties(){

        // try to get the properties from the server configuration file first.

        // get the length of the client id.
        const std::string& oauth2_client_id_length_str = granada::util::application::GetProperty(entity_keys::oauth2_client_id_length);
        if (oauth2_client_id_length_str.empty()){
          client_id_length_ = nonce_lengths::oauth2_client_id;
        }else{
          try{
            //client_id_length_ = std::stoi(oauth2_client_id_length_str);
            client_id_length_ = nonce_lengths::oauth2_client_id;
          }catch(const std::logic_error& e){
            client_id_length_ = nonce_lengths::oauth2_client_id;
          }
        }

        // get the name of the client's value namespace.
        cache_namespace_.assign(granada::util::application::GetProperty(entity_keys::oauth2_client_value_namespace));
        if (cache_namespace_.empty()){
          cache_namespace_.assign(cache_namespaces::oauth2_client_value);
        }
      }



      //////////////////////////////////////////////////
      // OAuth2 User
      ////

      std::mutex OAuth2User::oauth2_user_creation_mtx_;
      std::string OAuth2User::cache_namespace_;

      bool OAuth2User::Create(const std::string& username, std::string& password, const web::json::value& roles){
        username_.assign(username);
        
        const std::string& hash(this->hash());

        // save with unique username,
        // check if it does not already exist one user with the same username.
        oauth2_user_creation_mtx_.lock();
        if (Exists()){
          oauth2_user_creation_mtx_.unlock();
          return false;
        }else{
          // user with that username does not already exist,
          // save it.
          cache()->Write(hash, entity_keys::oauth2_user_username, username);
          oauth2_user_creation_mtx_.unlock();

          // save user properties.
          const std::string& key = cryptograph()->Encrypt(username,password);
          key_.assign(key);
          cache()->Write(hash, entity_keys::oauth2_user_key, key);
          std::string roles_str;
          try{
            roles_str = roles.serialize();
            roles_ = roles;
          }catch(const web::json::json_exception& e){
            roles_str = "{}";
            roles_ = web::json::value::parse(roles_str);
          }
          cache()->Write(hash, entity_keys::oauth2_user_roles, roles_str);
          cache()->Write(hash, entity_keys::oauth2_user_creation_time, granada::util::time::stringify(std::time(nullptr)));
          return true;
        }
      }

      void OAuth2User::Load(){
        if (!username_.empty() && Exists()){
          const std::string& hash(this->hash());

          // load user's properties.
          key_.assign(cache()->Read(hash, entity_keys::oauth2_user_key));
          std::string roles_str(cache()->Read(hash, entity_keys::oauth2_user_roles));

          try{
            roles_ = web::json::value::parse(roles_str);
          }catch(const web::json::json_exception& e){
            roles_str = "{}";
            roles_ = web::json::value::parse(roles_str);
          }

          const std::string& creation_time_str(cache()->Read(hash, entity_keys::oauth2_user_creation_time));
          creation_time_ = granada::util::time::parse(creation_time_str);
        }else{
          username_.assign("");
        }
      }

      void OAuth2User::Load(const std::string& identifier){
        if (!identifier.empty()){
          username_.assign(identifier);
          Load();
        }
      }


      bool OAuth2User::CorrectCredentials(std::string password){
        std::string decrypted_key = cryptograph()->Decrypt(key_,password);
        if (decrypted_key.length()>username_.length()){
          decrypted_key.erase(decrypted_key.begin()+username_.length(),decrypted_key.end());
        }
        if (decrypted_key == username_){
          return true;
        }
        return false;
      }

      bool OAuth2User::Delete(const std::string& password){
        if (cryptograph()->Decrypt(key_,password) == username_){
          cache()->Destroy(hash());
          return true;
        }
        return false;
      }

      void OAuth2User::LoadProperties(){
        // try to get the properties from the server configuration file first.

        // get the name of the user's value namespace.
        cache_namespace_.assign(granada::util::application::GetProperty(entity_keys::oauth2_user_value_namespace));
        if (cache_namespace_.empty()){
          cache_namespace_.assign(cache_namespaces::oauth2_user_value);
        }
      }



      //////////////////////////////////////////////////
      // OAuth2 Code
      ////

      std::mutex OAuth2Code::oauth2_code_creation_mtx_;
      std::string OAuth2Code::cache_namespace_;
      int OAuth2Code::code_length_;

      void OAuth2Code::Load(){
        if (!code_.empty() && Exists()){
          std::string hash = this->hash();

          // load code's properties.
          client_id_.assign(cache()->Read(hash, entity_keys::oauth2_code_client_id));
          username_.assign(cache()->Read(hash, entity_keys::oauth2_code_username));
          std::string roles_str(cache()->Read(hash, entity_keys::oauth2_code_roles));
          granada::util::string::split(roles_str, '+', roles_);
          std::string creation_time_str(cache()->Read(hash, entity_keys::oauth2_code_creation_time));
          creation_time_ = granada::util::time::parse(creation_time_str);
        }else{
          code_.assign("");
        }
      }

      void OAuth2Code::Load(const std::string& identifier){
        if (!identifier.empty()){
          code_.assign(identifier);
          Load();
        }
      };

      void OAuth2Code::Create(const std::string& client_id, const std::string& roles, const std::string& username){
        code_ = nonce_generator()->generate(code_length_);
        std::string hash = this->hash();

        // save with unique code,
        // check if it does not already exist one code with the same code.
        oauth2_code_creation_mtx_.lock();
        if (Exists()){
          oauth2_code_creation_mtx_.unlock();
          Create(client_id,roles,username);
        }else{

          // code does not exists, store it.
          cache()->Write(hash, entity_keys::oauth2_code_code, code_);
          oauth2_code_creation_mtx_.unlock();

          client_id_.assign(client_id);
          username_.assign(username);
          granada::util::string::split(roles,',',roles_);

          // store other useful values associated to code.
          cache()->Write(hash, entity_keys::oauth2_code_username, username_);
          cache()->Write(hash, entity_keys::oauth2_code_roles, roles);
          cache()->Write(hash, entity_keys::oauth2_code_client_id, client_id_);
          cache()->Write(hash, entity_keys::oauth2_code_creation_time, granada::util::time::stringify(std::time(nullptr)));
        }
      }

      void OAuth2Code::Delete(){
        cache()->Destroy(hash());
      }

      void OAuth2Code::LoadProperties(){
        // try to get the properties from the server configuration file first.

        // get the length of the code.
        std::string oauth2_code_length_str = granada::util::application::GetProperty(entity_keys::oauth2_code_length);
        if (oauth2_code_length_str.empty()){
          code_length_ = nonce_lengths::oauth2_code;
        }else{
          try{
            code_length_ = std::stoi(oauth2_code_length_str);
          }catch(const std::logic_error& e){
            code_length_ = nonce_lengths::oauth2_code;
          }
        }

        // get the name of the code's value namespace.
        cache_namespace_.assign(granada::util::application::GetProperty(entity_keys::oauth2_code_value_namespace));
        if (cache_namespace_.empty()){
          cache_namespace_.assign(cache_namespaces::oauth2_code_value);
        }
      }



      //////////////////////////////////////////////////
      // OAuth2 Authorization
      ////

      std::string OAuth2Authorization::cache_namespace_;
      bool OAuth2Authorization::oauth2_use_refresh_token_;

      void OAuth2Authorization::LoadProperties(){
        // retrieve if we have to generate refresh_token when generating access_token.
        // If true when client request an access token a refresh token is also delivered.
        std::string oauth2_use_refresh_token_str = granada::util::application::GetProperty(entity_keys::oauth2_use_refresh_token);
        if (oauth2_use_refresh_token_str.empty()){
          oauth2_use_refresh_token_str = default_strings::oauth2_use_refresh_token;
        }
        if (oauth2_use_refresh_token_str == entity_keys::_true){
          oauth2_use_refresh_token_ = true;
        }else{
          oauth2_use_refresh_token_ = false;
        }

        // get the name of the OAuth 2.0 authorization namespace.
        cache_namespace_.assign(granada::util::application::GetProperty(entity_keys::oauth2_authorization_namespace));
        if (cache_namespace_.empty()){
          cache_namespace_.assign(cache_namespaces::oauth2_authorization);
        }
      };


      granada::http::oauth2::OAuth2Parameters OAuth2Authorization::Grant(web::http::http_request &request, web::http::http_response& response){
        granada::http::oauth2::OAuth2Parameters oauth2_response;
        try{
          // if grant_type=refresh_token use grant type code as we will use the same resources and
          // the response will be the same as a code grant type.
          if (oauth2_parameters_.grant_type == oauth2_strings::refresh_token){
            oauth2_parameters_.grant_type = entity_keys::oauth2_code_code;
            oauth2_parameters_.code = oauth2_parameters_.refresh_token;
          }

          // check client application validity.
          std::unique_ptr<granada::http::oauth2::OAuth2Client> oauth2_client;
          CheckClient(oauth2_client,oauth2_response);

          if (oauth2_response.error.empty()){

            // the provided client is valid, its redirect URI also.

            // when we will have the response, redirect the user to a client redirection URI.
            oauth2_response.redirect_uri = oauth2_parameters_.redirect_uri;

            // if a state has been provided return it.
            // state prevents agains CSRF attacks.
            oauth2_response.state = oauth2_parameters_.state;

            // Now check the validity of the provided code, client credentials
            // or user credentials, depending on the grant type and the
            // provided credentials.

            // user, owner of the resources.
            std::unique_ptr<granada::http::oauth2::OAuth2User> oauth2_user;
            // used in case the user provided a code as grant.
            std::unique_ptr<granada::http::oauth2::OAuth2Code> oauth2_code;
            // session of the user in the authorization server.
            std::unique_ptr<granada::http::session::Session> oauth2_user_session;

            CheckCredentials(oauth2_client.get(),oauth2_user,oauth2_code,oauth2_user_session,oauth2_response,request,response);

            if (oauth2_response.error.empty()){

              // create code or access_token.

              // URL encode can encode spaces as %20 or as +
              // so standarize it to +.
              if (!oauth2_parameters_.scope.empty()){
                std::deque<std::pair<std::string,std::string>> values;
                values.push_back(std::make_pair(" ","+"));
                granada::util::string::replace(oauth2_parameters_.scope,values,"","");
              }

              // get the asked roles.
              std::vector<std::string> roles;
              if (oauth2_parameters_.code.empty()){
                granada::util::string::split(oauth2_parameters_.scope, '+', roles);
              }else{
                roles = oauth2_code->GetRoles();
              }

              // check if client is allowed to have the demanded scope/roles.
              if (CheckRoleAllowance(roles, oauth2_client.get(), oauth2_user.get())){
                if (oauth2_parameters_.response_type == oauth2_strings::code){
                  // respond with the requested code.
                  CreateCode(oauth2_user_session, oauth2_code, oauth2_user.get(),oauth2_response,request,response);
                  oauth2_parameters_.code = oauth2_code->GetCode();
                  oauth2_parameters_.username = oauth2_code->GetUsername();
                }else{
                  // respond with an access tokrn.
                  CreateAccessToken(roles,oauth2_user_session,oauth2_user.get(),oauth2_code,oauth2_response,request,response);
                }

                if (oauth2_response.error.empty()){
                  // store user, client, code and access token relation.
                  cache()->Write(hash(),"0");
                }
              }else{
                oauth2_response.error = oauth2_errors::invalid_scope;
                oauth2_response.error_description = oauth2_errors_description::invalid_scope;
              }
            }
          }
        }catch(const std::exception& e){
          // server error.
          oauth2_response.error = oauth2_errors::server_error;
          oauth2_response.error_description = oauth2_errors_description::server_error;
        }
        return oauth2_response;
      }


      void OAuth2Authorization::CheckClient(std::unique_ptr<granada::http::oauth2::OAuth2Client>& oauth2_client,
                                            granada::http::oauth2::OAuth2Parameters& oauth2_response){

        // check if client is registered
        oauth2_client = factory()->OAuth2Client_unique_ptr(oauth2_parameters_.client_id);
        if (oauth2_client->GetId().empty()){
          // Authorization error client is not valid.
          // Take the uri in the referer as the redirect uri.
          oauth2_response.error = oauth2_errors::invalid_client;
          oauth2_response.error_description = oauth2_errors_description::invalid_client;
        }else{
          // check if client redirect uri corresponds to the given or the assigned one.
          bool redirect_uri_exists = false;
          if (oauth2_parameters_.redirect_uri.empty() && oauth2_client->GetRedirectURIs().size() > 0){
            // if no given redirect uri, assign the first one of the client
            oauth2_parameters_.redirect_uri = oauth2_client->GetRedirectURIs()[0];
            redirect_uri_exists = true;
          }else{
            // redirect uri given, check if the client has it in its redirect uris collection.
            if (oauth2_client->HasRedirectURI(oauth2_parameters_.redirect_uri)){
              redirect_uri_exists = true;
            }
          }
          if (redirect_uri_exists){
            // we can redirect to a URI related to the client.
            if (oauth2_parameters_.response_type != oauth2_strings::code
              && oauth2_parameters_.response_type != oauth2_strings::token
              && oauth2_parameters_.grant_type != oauth2_strings::authorization_code){

              oauth2_response.error = oauth2_errors::unsupported_response_type;
              oauth2_response.error_description = oauth2_errors_description::unsupported_response_type;
            }
          }else{
            // Authorization error, the given redirect uri does not match with
            // the uri of the registered client.
            // Take the uri in the referer as the redirect uri.
            oauth2_response.error = oauth2_errors::invalid_grant;
            oauth2_response.error_description = oauth2_errors_description::invalid_grant;
          }
        }
      }


      void OAuth2Authorization::CheckCredentials(granada::http::oauth2::OAuth2Client* oauth2_client,
                                                 std::unique_ptr<granada::http::oauth2::OAuth2User>& oauth2_user,
                                                 std::unique_ptr<granada::http::oauth2::OAuth2Code>& oauth2_code,
                                                 std::unique_ptr<granada::http::session::Session>& oauth2_user_session,
                                                 granada::http::oauth2::OAuth2Parameters& oauth2_response,
                                                 web::http::http_request& request,
                                                 web::http::http_response& response){

        if (oauth2_parameters_.grant_type == oauth2_strings::authorization_code){
          // check if provided code is valid.
          if (oauth2_parameters_.code.empty()){
            oauth2_response.error = oauth2_errors::access_denied;
            oauth2_response.error_description = oauth2_errors_description::access_denied;
            return;
          }
          oauth2_code = factory()->OAuth2Code_unique_ptr(oauth2_parameters_.code);
          if (oauth2_code->GetCode().empty()){
            oauth2_response.error = oauth2_errors::access_denied;
            oauth2_response.error_description = oauth2_errors_description::access_denied;
            return;
          }
          oauth2_user = factory()->OAuth2User_unique_ptr(oauth2_code->GetUsername());
          if (oauth2_user->GetUsername().empty()){
            oauth2_response.error = oauth2_errors::access_denied;
            oauth2_response.error_description = oauth2_errors_description::access_denied;
            return;
          }
        }else{
          // check client credentials.
          if (!oauth2_parameters_.client_secret.empty() && !oauth2_client->CorrectCredentials(oauth2_parameters_.client_secret)){
            oauth2_response.error = oauth2_errors::unauthorized_client;
            oauth2_response.error_description = oauth2_errors_description::unauthorized_client;
            return;
          }else{
            // check auth session credentials.
            if (!oauth2_parameters_.authorize.empty() && oauth2_parameters_.authorize == oauth2_strings_2::authorize){
              oauth2_user_session = session_factory()->Session_unique_ptr(request,response);
              // a session will only be retrieved if it's valid.
              oauth2_parameters_.username = oauth2_user_session->roles()->GetProperty(entity_keys::oauth2_session_role,entity_keys::oauth2_session_role_username);
              if (oauth2_parameters_.username.empty()){
                oauth2_user_session->Close();
                oauth2_response.error = oauth2_errors::access_denied;
                oauth2_response.error_description = oauth2_errors_description::access_denied;
                return;
              }else{
                // if user provided check if it exists.
                oauth2_user = factory()->OAuth2User_unique_ptr(oauth2_parameters_.username);
                if (oauth2_user->GetUsername().empty()){
                  // the user provided a username but the
                  // user with that username does not exists, do not continue.
                  oauth2_response.error = oauth2_errors::access_denied;
                  oauth2_response.error_description = oauth2_errors_description::access_denied;
                  return;
                }
              }
            }else{
              // check user credentials.
              if (oauth2_parameters_.password.empty() || oauth2_parameters_.username.empty()){
                oauth2_response.error = oauth2_errors::access_denied;
                oauth2_response.error_description = oauth2_errors_description::access_denied;
                return;
              }else{
                // if user provided check if it exists.
                oauth2_user = factory()->OAuth2User_unique_ptr(oauth2_parameters_.username);
                if (oauth2_user->GetUsername().empty() || !oauth2_user->CorrectCredentials(oauth2_parameters_.password)){
                  // the user provided a username but the
                  // user with that username does not exists, do not continue.
                  oauth2_response.error = oauth2_errors::access_denied;
                  oauth2_response.error_description = oauth2_errors_description::access_denied;
                  return;
                }
              }
            }
          }
        }
      }



      void OAuth2Authorization::CreateCode(std::unique_ptr<granada::http::session::Session>& oauth2_user_session,
                                           std::unique_ptr<granada::http::oauth2::OAuth2Code>& oauth2_code,
                                           granada::http::oauth2::OAuth2User* oauth2_user,
                                           granada::http::oauth2::OAuth2Parameters& oauth2_response,
                                           web::http::http_request& request,
                                           web::http::http_response& response){
        // Authorization Code Grant
        oauth2_code = factory()->OAuth2Code_unique_ptr();
        oauth2_code->Create(oauth2_parameters_.client_id,oauth2_parameters_.scope,oauth2_parameters_.username);
        const std::string& code = oauth2_code->GetCode();
        if ( code.empty() ){
          // problem with credentials
          oauth2_response.error = oauth2_errors::server_error;
          oauth2_response.error_description = oauth2_errors_description::server_error;
        }else{
          oauth2_response.code = code;
          AssignRolesToOAuth2UserSession(oauth2_user_session, oauth2_user->GetRoles(),request,response);
        }
      }

      void OAuth2Authorization::CreateAccessToken(std::vector<std::string>& roles,
                                                  std::unique_ptr<granada::http::session::Session>& oauth2_user_session,
                                                  granada::http::oauth2::OAuth2User* oauth2_user,
                                                  std::unique_ptr<granada::http::oauth2::OAuth2Code>& oauth2_code,
                                                  granada::http::oauth2::OAuth2Parameters& oauth2_response,
                                                  web::http::http_request& request,
                                                  web::http::http_response& response){
        // Client session. Resource access session.
        std::unique_ptr<granada::http::session::Session> oauth2_client_session = session_factory()->Session_unique_ptr();
        oauth2_client_session->Open();

        // set session roles
        AssignRolesToClientSession(roles,oauth2_user->GetRoles(),oauth2_client_session.get());
        oauth2_response.access_token = oauth2_client_session->GetToken();
        oauth2_response.token_type = oauth2_strings::bearer;
        oauth2_response.scope = oauth2_parameters_.scope;

        oauth2_parameters_.username = oauth2_user->GetUsername();
        oauth2_parameters_.access_token = oauth2_response.access_token;

        if (oauth2_parameters_.grant_type == oauth2_strings::authorization_code){
          // Access Token Request
          if (oauth2_use_refresh_token_){
            // create refresh token
            // The refresh token can be used to obtain new access tokens using the same
            // authorization grant.
            CreateRefreshToken(oauth2_client_session.get(), oauth2_code, oauth2_response);
          }
        }else{
          AssignRolesToOAuth2UserSession(oauth2_user_session, oauth2_user->GetRoles(),request,response);
        }
      }

      void OAuth2Authorization::CreateRefreshToken(granada::http::session::Session* oauth2_client_session,
                                                   std::unique_ptr<granada::http::oauth2::OAuth2Code>& oauth2_code,
                                                   granada::http::oauth2::OAuth2Parameters& oauth2_response){

        const long& session_timeout = oauth2_client_session->GetSessionTimeout();
        try{
          oauth2_response.expires_in.assign(std::to_string(session_timeout));
        }catch(const std::exception& e){
          oauth2_response.expires_in.assign("-1");
        }

        // generate a refresh token,
        // for us it's the same as generating an OAuth 2.0 code.
        oauth2_code = factory()->OAuth2Code_unique_ptr();
        oauth2_code->Create(oauth2_parameters_.client_id,oauth2_parameters_.scope,oauth2_parameters_.username);
        std::string refresh_token = oauth2_code->GetCode();
        if ( !refresh_token.empty() ){
          oauth2_response.refresh_token = refresh_token;
        }
      }

      bool OAuth2Authorization::CheckRoleAllowance(std::vector<std::string>& roles,
                                                   granada::http::oauth2::OAuth2Client* oauth2_client,
                                                   granada::http::oauth2::OAuth2User* oauth2_user){
        // check if the client session can have the asked roles
        // or if it is not allowed to have them. Check in the
        // registered client roles and in the user roles.
        std::string role;
        for (auto it=roles.begin(); it != roles.end(); ++it){
          role = *it;
          if (!oauth2_client->HasRole(role) || !oauth2_user->HasRole(role)){
            return false;
          }
        }
        return true;
      }

      void OAuth2Authorization::AssignRolesToClientSession(std::vector<std::string>& roles,
                                                           const web::json::value& user_roles,
                                                           granada::http::session::Session* oauth2_client_session){
        std::string role;
        web::json::value role_properties;
        if (!user_roles.is_null()){
          for (auto it = roles.begin(); it != roles.end(); ++it){
            role.assign(*it);
            if (user_roles.has_field(role)){
              oauth2_client_session->roles()->Add(role);
              role_properties = user_roles.at(role);
              if (role_properties.is_object()){
                for(auto it = role_properties.as_object().cbegin(); it != role_properties.as_object().cend(); ++it){
                  const std::string& property_name = it->first;
                  const web::json::value& property_value = it->second;
                  if (property_value.is_string()){
                    oauth2_client_session->roles()->SetProperty(role, property_name, property_value.as_string());
                  }
                }
              }
            }
          }
        }
      }

      void OAuth2Authorization::AssignRolesToOAuth2UserSession(std::unique_ptr<granada::http::session::Session>& oauth2_user_session,
                                                               const web::json::value& user_roles,
                                                                web::http::http_request& request,
                                                                web::http::http_response& response){
        if (oauth2_user_session.get() == nullptr){
          oauth2_user_session = session_factory()->Session_unique_ptr(request,response);
        }

        oauth2_user_session->roles()->Add(entity_keys::oauth2_session_role);
        oauth2_user_session->roles()->SetProperty(entity_keys::oauth2_session_role,entity_keys::oauth2_session_role_username,oauth2_parameters_.username);

        if(!user_roles.is_null()){
          for(auto it = user_roles.as_object().cbegin(); it != user_roles.as_object().cend(); ++it){
            const std::string& role_name = it->first;
            const web::json::value& role_properties = it->second;

            oauth2_user_session->roles()->Add(role_name);

            // assign properties to role
            if (role_properties.is_object()){
              for(auto it2 = role_properties.as_object().cbegin(); it2 != role_properties.as_object().cend(); ++it2){
                const std::string& property_name = it2->first;
                const web::json::value& property_value = it2->second;
                if (property_value.is_string()){
                  oauth2_user_session->roles()->SetProperty(role_name, property_name, property_value.as_string());
                }
              }
            }
          }
        }
      }

      web::json::value OAuth2Authorization::Information(){
        std::string json_str = "";
        if (oauth2_parameters_.client_id.empty()){
          // give information about all the clients authorized by the user.
          std::vector<std::string> clients_ids;
          const std::string& expression = cache_namespace_ + oauth2_parameters_.username + ":*:*:*";
          std::unique_ptr<granada::cache::CacheHandlerIterator> cache_iterator = cache()->make_iterator(expression);
          std::string key;
          std::string client_id;
          while(cache_iterator->has_next()){
            key = cache_iterator->next();
            std::vector<std::string> splitted_key;
            granada::util::string::split(key,':',splitted_key);
            if (splitted_key.size() > 2){
              client_id = splitted_key[2];
              if (!client_id.empty()){
                if (std::find(clients_ids.begin(), clients_ids.end(), client_id) == clients_ids.end()){
                  // client not already reported, retrieve its application name and report it.
                  const std::unique_ptr<granada::http::oauth2::OAuth2Client>& oauth2_client = factory()->OAuth2Client_unique_ptr(client_id);
                  json_str += ",{\"client_id\":\"" + client_id + "\",\"application_name\":\"" + oauth2_client->GetApplicationName() + "\"}";
                  clients_ids.push_back(client_id);
                }
              }
            }
          }
        }else{
          // give information about the client with the given id authorized to the user.
          std::unique_ptr<granada::http::oauth2::OAuth2Client> oauth2_client = factory()->OAuth2Client_unique_ptr();
          oauth2_client->SetId(oauth2_parameters_.client_id);
          if (oauth2_client->Exists()){
            // get all the codes and the access_tokens linked to that client and user.
            std::vector<std::string> codes;
            const std::string& expression = cache_namespace_ + oauth2_parameters_.username + ":" + oauth2_parameters_.client_id + ":*:*";
            std::unique_ptr<granada::cache::CacheHandlerIterator> cache_iterator = cache()->make_iterator(expression);
            std::string key;
            std::string code;
            while(cache_iterator->has_next()){
              key = cache_iterator->next();
              std::vector<std::string> splitted_key;
              granada::util::string::split(key,':',splitted_key);
              if (splitted_key.size() > 3){
                code = splitted_key[3];
                if (!code.empty()){
                  if (std::find(codes.begin(), codes.end(), code) == codes.end()){
                    // code not already reported, report it.
                    json_str += ",\"" + code + "\"";
                    codes.push_back(code);
                  }
                }
              }
            }
          }else{
            granada::http::oauth2::OAuth2Parameters oauth2_response;
            oauth2_response.error = oauth2_errors::unauthorized_client;
            oauth2_response.error_description = oauth2_errors_description::unauthorized_client;
            return oauth2_response.to_json();
          }
        }

        // convert to json.
        if (json_str.length() > 1){
          json_str[0] = '[';
          json_str = "{\"data\":" + json_str + "]}";
        }
        web::json::value json;
        try{
          json = web::json::value::parse(json_str);
        }catch(const web::json::json_exception& e){
          json_str = "{\"data\":[]}";
          json = web::json::value::parse(json_str);
        }
        return json;
      }


      web::json::value OAuth2Authorization::Delete(){
        web::json::value json;

        std::unique_ptr<granada::http::oauth2::OAuth2Client> oauth2_client = factory()->OAuth2Client_unique_ptr();
        oauth2_client->SetId(oauth2_parameters_.client_id);
        if (oauth2_client->Exists()){
          // remove all codes and access_token from a client.
          std::vector<std::string> keys;
          std::vector<std::string> codes;
          std::vector<std::string> access_tokens;
          const std::string& expression = cache_namespace_ + oauth2_parameters_.username + ":" + oauth2_parameters_.client_id + ":*:*";
          std::unique_ptr<granada::cache::CacheHandlerIterator> cache_iterator = cache()->make_iterator(expression);
          std::string key;
          std::string code;
          std::string access_token;
          std::unique_ptr<granada::http::oauth2::OAuth2Code> oauth2_code = factory()->OAuth2Code_unique_ptr();
          while(cache_iterator->has_next()){
            key = cache_iterator->next();
            // a key is formed with namespace + username + client_id + code + access_token
            // so we split it to have access to the code and the access_token.
            std::vector<std::string> splitted_key;
            granada::util::string::split(key,':',splitted_key);

            if (splitted_key.size() > 4){

              // remove code and report it as removed
              code = splitted_key[3];
              if (!code.empty()){
                if (std::find(codes.begin(), codes.end(), code) == codes.end()){
                  oauth2_code->SetCode(code);
                  oauth2_code->Delete();
                  codes.push_back(code);
                }
              }

              // remove access_token and report it as removed
              access_token = splitted_key[4];
              if (!access_token.empty()){
                if (std::find(access_tokens.begin(), access_tokens.end(), access_token) == access_tokens.end()){
                  std::unique_ptr<granada::http::session::Session> session = session_factory()->Session_unique_ptr(access_token);
                  session->Close();
                  access_tokens.push_back(access_token);
                }
              }
            }

            // store key to remove.
            keys.push_back(key);
          }

          // remove OAuth 2.0 authorizations
          for (auto it = keys.begin(); it != keys.end(); ++it){
            cache()->Destroy(*it);
          }
        }else{
          granada::http::oauth2::OAuth2Parameters oauth2_response;
          oauth2_response.error = oauth2_errors::unauthorized_client;
          oauth2_response.error_description = oauth2_errors_description::unauthorized_client;
          return oauth2_response.to_json();
        }

        oauth2_parameters_.client_id = "";
        json = Information();

        return json;
      }


    }
  }
}
