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
  * Session that allows to store and retrieve string values.
  * Uses cache/redis_cache_driver.h
  *
  */

#pragma once
#include <unordered_map>
#include "session.h"
#include "redis_roles.h"
#include "redis_session_handler.h"
#include "granada/defaults.h"
#include "granada/cache/redis_cache_driver.h"

namespace granada{
  namespace http{
    namespace session{
      class RedisStorageSession : public Session
      {
        public:

          /**
           * Constructor
           */
          RedisStorageSession();


          /**
           * Constructor.
           * Loads session.
           * Retrieves the token of the session from the HTTP request
           * and loads a session using the session handler.
           * If session does not exist or token is not found
           * a new session is created.
           * This constructor is recommended for sessions that store token in cookie
           *
           * @param  request  Http request.
           * @param  response Http response.
           */
          RedisStorageSession(web::http::http_request &request,web::http::http_response &response);


          /**
           * Constructor.
           * Loads session.
           * Retrieves the token of the session from the HTTP request
           * and loads a session using the session handler.
           * If session does not exist or token is not found
           * a new session is created.
           * This constructor is recommended for sessions that use get and post values.
           * 
           * @param  request  Http request.
           */
          RedisStorageSession(web::http::http_request &request);


          /**
           * Constructor.
           * Loads a session with the given token using the session handler.
           * Use this loader if you have the token and you are not using cookies.
           * 
           * @param token Session token.
           */
          RedisStorageSession(const std::string& token);


          /**
           * Set the value of the sessions, may be overriden in case we want to
           * make other actions.
           * @param session
           */
          void set(granada::http::session::Session* session){
            (*this) = (*((granada::http::session::RedisStorageSession*)session));
            roles_->SetSession(this);
          };


          /**
           * Set the value of the sessions, may be overriden in case we want to
           * make other actions.
           * 
           * @param token         Session token.
           * @param update_time   Session update time.
           */
          void set(const std::string& token,const std::time_t& update_time){
            LoadProperties();
            SetToken(token);
            SetUpdateTime(update_time);
          };


          /**
           * Closes a session deleting it.
           * And call all the close callback functions.
           */
          void Close();


          /**
           * Updates a session, updating the session update time to now and saving it.
           * That means the session will timeout in now + timeout. It will keep
           * the session alive.
           */
          void Update();


          /**
           * Returns the roles of a session.
           * @return The roles of the session.
           */
          std::shared_ptr<granada::http::session::Roles> roles(){ return roles_; };


          /**
           * Returns a pointer to the collection of functions
           * that are called when closing the session.
           * 
           * @return  Pointer to the collection of functions that are
           *          called when session is closed.
           */
          granada::Functions* close_callbacks(){
            return close_callbacks_.get();
          };


          /**
           * Write session data.
           * @param key   Key or name of the data.
           * @param value Data as string.
           */
          void Write(const std::string& key, const std::string& value);


          /**
           * Read session data.
           * @param  key Key or name of the data.
           * @return     Data as string.
           */
          const std::string Read(const std::string& key);


          /**
           * Destroy session data with given key.
           * @param key Data key or name.
           */
          void Destroy(const std::string& key);


        private:

          /**
           * Returns the pointer of Session Handler that manages the session.
           * @return Session Handler.
           */
          granada::http::session::SessionHandler* session_handler(){
            return session_handler_.get();
          };


          /**
           * Hanlder of the sessions lifetime, and where all the application sessions are stored.
           */
          static std::unique_ptr<granada::http::session::SessionHandler> session_handler_;


          /**
           * Loads the properties as session_clean_extra_timeout_.
           */
          void LoadProperties();


          /**
           * Manager of the roles of the session and its properties
           */
          std::shared_ptr<granada::http::session::Roles> roles_;


          /**
           * Manager of the roles of the session and its properties
           */
          static std::unique_ptr<granada::Functions> close_callbacks_;


          /**
           * Manager of the storage, and contains
           * the data stored.
           */
          static std::unique_ptr<granada::cache::CacheHandler> cache_;


          /**
           * Used for determining if the session is garbage.
           * In case the session is timed out since x seconds indicated
           * in the "session_clean_extra_timeout" property
           * If no property indicated, it will take entity_keys::session_clean_extra_timeout.
           */
          long session_clean_extra_timeout_;


      };
    }
  }
}
