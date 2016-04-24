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
#include "session.h"
#include "map_roles.h"
#include "redis_session_handler.h"
#include "granada/cache/redis_cache_driver.h"

namespace granada{
  namespace http{
    namespace session{
      class RedisStorageSession : public Session
      {
        public:

          /**
           * Constructors
           */
          RedisStorageSession(){};
          RedisStorageSession(web::http::http_request &request,web::http::http_response &response);
          RedisStorageSession(web::http::http_request &request);
          RedisStorageSession(const std::string& token);

          // override
          void set(granada::http::session::Session* session){
            (*this) = (*((granada::http::session::RedisStorageSession*)session));
            roles_->SetSession(this);
          };

          // override
          void set(const std::string& token,const std::time_t& update_time){
            LoadProperties();
            SetToken(token);
            SetUpdateTime(update_time);
          };

          // override
          void Open();

          // override
          void Close();

          // override
          void Update();

          // override
          const bool IsValid();

          //override
          const bool IsGarbage();

          // override
          granada::http::session::Roles* roles(){ return roles_.get(); };


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

          granada::http::session::SessionHandler* session_handler(){ return session_handler_.get(); };

          /**
           * Hanlder of the sessions lifetime, and where all the application sessions are stored.
           */
          static std::unique_ptr<granada::http::session::RedisSessionHandler> session_handler_;

          /**
           * Loads the properties as session_clean_extra_timeout_.
           */
          void LoadProperties();

          /**
           * Manager of the roles of the session and its properties
           */
          std::shared_ptr<granada::http::session::Roles> roles_;

          /**
           * Manager of the storage, and contains
           * the data stored.
           */
          static std::unique_ptr<granada::cache::RedisCacheDriver> cache_;


          /**
           * Used for determining if the session is garbage.
           * In case the session is timed out since x seconds indicated
           * in the "session_clean_extra_timeout" property
           * If no property indicated, it will take DEFAULT_SESSION_CLEAN_EXTRA_TIMEOUT.
           */
          long session_clean_extra_timeout_;


          /**
           * This default value is taken in case "session_clean_extra_timeout" property is not found.
           */
          static long DEFAULT_SESSION_CLEAN_EXTRA_TIMEOUT;

      };
    }
  }
}
