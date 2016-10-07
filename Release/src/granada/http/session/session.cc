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
#include "granada/http/session/session.h"

namespace granada{
  namespace http{
    namespace session{


////
// static membesr of Session
//
      std::vector<std::string> Session::DEFAULT_SESSIONS_TOKEN_SUPPORT = {entity_keys::session_token_support,default_strings::session_second_token_support};
      std::string Session::token_label_;
      std::string Session::application_session_token_support_;
      long Session::application_session_timeout_ = -1;
      long Session::session_garbage_extra_timeout_ = 0;
      std::mutex Session::session_exists_mtx_;
//
////


      void Session::Open(){
        // if token already exist, delete the copy of the "old" session
        // from where it is stored, so its not used again.
        Close();

        // generate token
        token_.assign(session_handler()->GenerateToken());
        Session::session_exists_mtx_.lock();
        // check if session do not already exist.
        if (session_handler()->SessionExists(token_)){
          Session::session_exists_mtx_.unlock();

          // a session with this token already exist!
          // repeat the process of opening a new session.
          Open();
        }else{

          // session is created, update it, for example the sesison update time.
          Update();
          Session::session_exists_mtx_.unlock();
        }
      }


      void Session::Open(web::http::http_response &response){

        // open session
        Open();
        if (session_token_support_ == entity_keys::session_cookie){

          // add cookie with token
          response.headers().add(entity_keys::session_set_cookie, token_label() + "=" + token_ + "; path=/");
        }
      }


      void Session::Update(){

        // set the update time to now.
        update_time_ = std::time(nullptr);

        // save the session wherever all the sessions are stored.
        session_handler()->SaveSession(this);
      }


      void Session::Close(){
        // removes a session from wherever sessions are stored.
        web::json::value session_json = to_json();
        close_callbacks()->CallAll(session_json);
        roles()->RemoveAll();
        session_handler()->DeleteSession(this);
      }


      const bool Session::IsTimedOut(){
        IsTimedOut(0);
      }


      const bool Session::IsTimedOut(const long int& extra_seconds){
        if ( application_session_timeout() < 0){
          return false;
        }else{
          return granada::util::time::is_timedout(update_time_,application_session_timeout(),extra_seconds);
        }
      }


      const bool Session::IsValid(){
        return !IsTimedOut(session_garbage_extra_timeout());
      }


      const bool Session::IsGarbage(){
        return !IsValid();
      }


      const long Session::GetSessionTimeout(){
        if (application_session_timeout()>-1){
          std::time_t now = std::time(nullptr);
          return application_session_timeout_ - (now - update_time_);
        }else{
          return -1;
        }
      }


      web::json::value Session::to_json(){
        web::json::value json = web::json::value::object();
        json[entity_keys::session_token] = web::json::value::string(token_);
        json[entity_keys::session_json_update_time] = web::json::value::string(granada::util::time::stringify(GetUpdateTime()));
        json[entity_keys::session_timeout] = web::json::value::string(granada::util::time::stringify(GetSessionTimeout()));
        return json;
      }


      void Session::LoadProperties(){
        const std::string& session_garbage_extra_timeout_str(granada::util::application::GetProperty(entity_keys::session_garbage_extra_timeout));
        if (session_garbage_extra_timeout_str.empty()){
          session_garbage_extra_timeout_ = default_numbers::session_session_garbage_extra_timeout;
        }else{
          try{
            session_garbage_extra_timeout_ = std::stol(session_garbage_extra_timeout_str);
          }catch(const std::logic_error& e){
            session_garbage_extra_timeout_ = default_numbers::session_session_garbage_extra_timeout;
          }
        }

        Session::token_label_.assign(granada::util::application::GetProperty(entity_keys::session_token_label));
        if (Session::token_label_.empty()){
          Session::token_label_.assign(default_strings::session_token_label);
        }

        Session::application_session_token_support_ = granada::util::application::GetProperty(entity_keys::session_token_support);

        const std::string& application_session_timeout_str = granada::util::application::GetProperty(entity_keys::session_timeout);
        if (application_session_timeout_str.empty()){
          Session::application_session_timeout_ = default_numbers::session_timeout;
        }else{
          try{
            Session::application_session_timeout_ = std::stol(application_session_timeout_str);
          }catch(const std::logic_error& e){
            Session::application_session_timeout_ = default_numbers::session_timeout;
          }
        }
      }


      const bool Session::LoadSession(const web::http::http_request &request,web::http::http_response &response){
        if (session_token_support_.empty()){
          if (application_session_token_support().empty()){
            // request token by default
            session_token_support_ = Session::DEFAULT_SESSIONS_TOKEN_SUPPORT[0];
          }else{
            session_token_support_ = application_session_token_support();
          }
        }

        // search and retrieve token from cookies.
        if (session_token_support_ == entity_keys::session_cookie){
          bool session_exists = false;
          const std::unordered_map<std::string, std::string>& cookies = granada::http::parser::ParseCookies(request);
          auto it = cookies.find(token_label());
          if (it != cookies.end()){
            std::string token = it->second;
            session_exists = LoadSession(token);
          }
          if(!session_exists){
            Open(response);
          }
          return true;
        }
        return LoadSession(request);
      }


      const bool Session::LoadSession(const web::http::http_request &request){
        if (session_token_support_.empty()){
          if (application_session_token_support().empty()){
            // request token by default
            session_token_support_ = Session::DEFAULT_SESSIONS_TOKEN_SUPPORT[1];
          }else{
            session_token_support_ = application_session_token_support();
          }
        }

        // retrieve token from body json.
        if (session_token_support_ == entity_keys::session_json){
          try{
            const web::json::value& obj = request.extract_json().get();
            const std::string& token = granada::util::json::as_string(obj,token_label());
            return LoadSession(token);
          }catch(const std::exception& e){}
        }else{
          // retrieve token from query string.
          if (session_token_support_ == entity_keys::session_query){
            const std::string& query_string = request.request_uri().query();
            try{
              std::unordered_map<std::string, std::string> parsed_query = granada::http::parser::ParseQueryString(query_string);
              const std::string& token = parsed_query[token_label()];
              return LoadSession(token);
            }catch(const std::exception& e){
              return false;
            }
          }
        }
        return false;
      }


      const bool Session::LoadSession(const std::string& token){
        if (!token.empty()){

          // use session handler to load session from wherever the sessions are stored.
          // If session is found the value of this session will be replaced by the
          // found session.
          session_handler()->LoadSession(token,this);
          if (!token_.empty()){
            // session found, update the session. For example the session update time,
            // so session is kept alive.
            Update();
            return true;
          }
        }
        return false;
      }




      const bool SessionRoles::Is(const std::string& role_name){
        return cache()->Exists(session_roles_hash(role_name));
      }


      const bool SessionRoles::Add(const std::string& role_name){
        // add only if role is not already added.
        if (!Is(role_name)){
          cache()->Write(session_roles_hash(role_name), "0", "0");
          session_->Update();
          return true;
        }
        return false;
      }


      void SessionRoles::Remove(const std::string& role_name){
        cache()->Destroy(session_roles_hash(role_name));
        session_->Update();
      }


      void SessionRoles::RemoveAll(){
        Remove("*");
        session_->Update();
      }


      void SessionRoles::SetProperty(const std::string& role_name, const std::string& key, const std::string& value){
        cache()->Write(session_roles_hash(role_name), key, value);
        session_->Update();
      }


      const std::string SessionRoles::GetProperty(const std::string& role_name, const std::string& key){
        return cache()->Read(session_roles_hash(role_name), key);
      }


      void SessionRoles::DestroyProperty(const std::string& role_name, const std::string& key){
        cache()->Destroy(session_roles_hash(role_name), key);
        session_->Update();
      }




      int SessionHandler::token_length_ = 32;
      double SessionHandler::clean_sessions_frequency_ = -1;


      const bool SessionHandler::SessionExists(const std::string& token){
        if (!token.empty()){
          return cache()->Exists(session_value_hash(token));
        }
        return false;
      }


      const std::string SessionHandler::GenerateToken(){
        return nonce_generator()->generate(token_length());
      }


      void SessionHandler::LoadSession(const std::string& token, granada::http::session::Session* virgin){
        if (!token.empty()){
          const time_t& update_time = granada::util::time::parse(cache()->Read(session_value_hash(token), entity_keys::session_update_time));
          virgin->set(token,update_time);
          if (!virgin->IsValid()){
            virgin->set("",0);
          }
        }
      }


      void SessionHandler::SaveSession(granada::http::session::Session* session){
        const std::string& token = session->GetToken();
        if (!token.empty()){
          const std::string& hash = session_value_hash(token);
          cache()->Write(hash, entity_keys::session_token, token);
          cache()->Write(hash, entity_keys::session_update_time, granada::util::time::stringify(session->GetUpdateTime()));
        }
      }


      void SessionHandler::DeleteSession(granada::http::session::Session* session){
        const std::string& token = session->GetToken();
        if (!token.empty()){
          const std::shared_ptr<granada::cache::CacheHandlerIterator>& cache_iterator = cache()->make_iterator("session:*" + token + "*");
          while(cache_iterator->has_next()){
            cache()->Destroy(cache_iterator->next());
          }
        }
      }


      void SessionHandler::CleanSessions(){
        const std::shared_ptr<granada::cache::CacheHandlerIterator>& cache_iterator = cache()->make_iterator(session_value_hash("*"));
        while(cache_iterator->has_next()){
          const std::string& key = cache_iterator->next();
          const std::shared_ptr<granada::http::session::Session>& session = checkpoint()->check(cache()->Read(key, entity_keys::session_token));
          const time_t& update_time = granada::util::time::parse(cache()->Read(key, entity_keys::session_update_time));
          session->SetUpdateTime(update_time);
          if (session->IsGarbage()){
            session->Close();
          }
        }
      }


      void SessionHandler::CleanSessions(bool recursive){
        if (clean_sessions_frequency()>-1){
          granada::util::time::sleep_seconds(clean_sessions_frequency());
          CleanSessions();
          
          // ... and clean sessions again.
          CleanSessions(true);
        }else{
          CleanSessions();
        }
      }


      void SessionHandler::LoadProperties(){
        const std::string& clean_sessions_frequency_str(granada::util::application::GetProperty(entity_keys::session_clean_frequency));
        if (clean_sessions_frequency_str.empty()){
          SessionHandler::clean_sessions_frequency_ = default_numbers::session_clean_sessions_frequency;
        }else{
          try{
            SessionHandler::clean_sessions_frequency_ = std::stod(clean_sessions_frequency_str);
          }catch(const std::exception& e){
            SessionHandler::clean_sessions_frequency_ = default_numbers::session_clean_sessions_frequency;
          }
        }
        const std::string& token_length_str(granada::util::application::GetProperty(entity_keys::session_token_length));
        if (token_length_str.empty()){
          SessionHandler::token_length_ = nonce_lengths::session_token;
        }else{
          try{
            SessionHandler::token_length_ = std::stoi(token_length_str);
          }catch(const std::exception& e){
            SessionHandler::token_length_ = nonce_lengths::session_token;
          }
        }
      }

    }
  }
}
