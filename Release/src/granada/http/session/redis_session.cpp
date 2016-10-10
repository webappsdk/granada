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

#include "granada/http/session/redis_session.h"

namespace granada{
  namespace http{
    namespace session{

      std::once_flag RedisSession::properties_flag_;
      std::unique_ptr<granada::http::session::SessionHandler> RedisSession::session_handler_(new granada::http::session::RedisSessionHandler());
      std::unique_ptr<granada::Functions> RedisSession::close_callbacks_(new granada::FunctionsMap());


      RedisSession::RedisSession(){
        std::call_once(RedisSession::properties_flag_, [this](){
          this->LoadProperties();
        });
        roles_ = std::unique_ptr<granada::http::session::SessionRoles>(new granada::http::session::RedisSessionRoles(this));
      }


      RedisSession::RedisSession(const web::http::http_request &request,web::http::http_response &response){
        std::call_once(RedisSession::properties_flag_, [this](){
          this->LoadProperties();
        });
        roles_ = std::unique_ptr<granada::http::session::SessionRoles>(new granada::http::session::RedisSessionRoles(this));
        Session::LoadSession(request,response);
      }


      RedisSession::RedisSession(const web::http::http_request &request){
        std::call_once(RedisSession::properties_flag_, [this](){
          this->LoadProperties();
        });
        roles_ = std::unique_ptr<granada::http::session::SessionRoles>(new granada::http::session::RedisSessionRoles(this));
        Session::LoadSession(request);
      }


      RedisSession::RedisSession(const std::string& token){
        std::call_once(RedisSession::properties_flag_, [this](){
          this->LoadProperties();
        });
        roles_ = std::unique_ptr<granada::http::session::SessionRoles>(new granada::http::session::RedisSessionRoles(this));
        Session::LoadSession(token);
      }


      std::unique_ptr<granada::cache::CacheHandler> RedisSessionRoles::cache_(new granada::cache::RedisCacheDriver());
      
      std::once_flag RedisSessionHandler::properties_flag_;
      std::unique_ptr<granada::cache::CacheHandler> RedisSessionHandler::cache_(new granada::cache::RedisCacheDriver());
      std::unique_ptr<granada::crypto::NonceGenerator> RedisSessionHandler::nonce_generator_(new granada::crypto::CPPRESTNonceGenerator());
      std::unique_ptr<granada::http::session::SessionCheckpoint> RedisSessionHandler::checkpoint_(new granada::http::session::RedisSessionCheckpoint());

    }
  }
}
