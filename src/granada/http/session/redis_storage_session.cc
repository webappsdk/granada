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
#include "granada/http/session/redis_storage_session.h"

namespace granada{
  namespace http{
    namespace session{

      // we use a session handler that use a map shared by all user to store the sessions.
      std::unique_ptr<granada::http::session::RedisSessionHandler> RedisStorageSession::session_handler_(new granada::http::session::RedisSessionHandler(std::shared_ptr<granada::http::session::RedisStorageSession>(new granada::http::session::RedisStorageSession())));
      std::unique_ptr<granada::cache::RedisCacheDriver> RedisStorageSession::cache_(new granada::cache::RedisCacheDriver());
      long RedisStorageSession::DEFAULT_SESSION_CLEAN_EXTRA_TIMEOUT = 0;

      RedisStorageSession::RedisStorageSession(){
        roles_ = std::shared_ptr<granada::http::session::Roles>(new granada::http::session::RedisRoles(this));
      }

      RedisStorageSession::RedisStorageSession(web::http::http_request &request,web::http::http_response &response){
        roles_ = std::shared_ptr<granada::http::session::Roles>(new granada::http::session::RedisRoles(this));
        LoadProperties();
        Session::LoadSession(request,response);
      }

      RedisStorageSession::RedisStorageSession(web::http::http_request &request){
        roles_ = std::shared_ptr<granada::http::session::Roles>(new granada::http::session::RedisRoles(this));
        LoadProperties();
        Session::LoadSession(request);
      }

      RedisStorageSession::RedisStorageSession(const std::string& token){
        roles_ = std::shared_ptr<granada::http::session::Roles>(new granada::http::session::RedisRoles(this));
        LoadProperties();
        Session::LoadSession(token);
      }


      void RedisStorageSession::LoadProperties(){
        Session::LoadProperties();
        std::string session_clean_extra_timeout_str(session_handler()->GetProperty("session_clean_extra_timeout"));
        if (session_clean_extra_timeout_str.empty()){
          session_clean_extra_timeout_ = RedisStorageSession::DEFAULT_SESSION_CLEAN_EXTRA_TIMEOUT;
        }else{
          try{
            session_clean_extra_timeout_ = std::stol(session_clean_extra_timeout_str);
          }catch(const std::exception& e){
            session_clean_extra_timeout_ = RedisStorageSession::DEFAULT_SESSION_CLEAN_EXTRA_TIMEOUT;
          }
        }
      }

      void RedisStorageSession::Update(){
        update_time_ = std::time(nullptr);
        granada::http::session::RedisStorageSession* redis_storage_session_ptr = new granada::http::session::RedisStorageSession();
        *redis_storage_session_ptr = *this;
        std::shared_ptr<granada::http::session::RedisStorageSession> redis_storage_session_shared_ptr(redis_storage_session_ptr);
        // save the session wherever all the sessions are stored.
        session_handler()->SaveSession(redis_storage_session_shared_ptr);
      }

      void RedisStorageSession::Close(){
        session_handler()->DeleteSession(this);
      }

      const std::string RedisStorageSession::Read(const std::string& key){
        if (token_.empty()){
          return std::string();
        }
        return cache_->Read("session:data:" + token_, key);
      }

      void RedisStorageSession::Write(const std::string& key, const std::string& value){
        if (!token_.empty()){
          cache_->Write("session:data:" + token_, key, value);
        }
      }

      void RedisStorageSession::Destroy(const std::string& key){
        if (!token_.empty()){
          cache_->Destroy("session:data:" + token_, key);
        }
      }
    }
  }
}
