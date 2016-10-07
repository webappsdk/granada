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
  * Abstract class for sessions that
  * allow to store and retrieve string values
  * associated with a session.
  * 
  */

#pragma once
#include "session.h"

namespace granada{
  namespace http{
    namespace session{


      /**
       * Abstract class for sessions that
       * allow to store and retrieve string values
       * associated with a session.
       */
      class StorageSession : public Session
      {

        public:

          /**
           * Constructor.
           */
          StorageSession(){};


          /**
           * Write session data.
           * @param key   Key or name of the data.
           * @param value Data as string.
           */
          virtual void Write(const std::string& key, const std::string& value);


          /**
           * Read session data.
           * @param  key Key or name of the data.
           * @return     Data as string.
           */
          virtual const std::string Read(const std::string& key);


          /**
           * Destroy session data with given key.
           * @param key Data key or name.
           */
          virtual void Destroy(const std::string& key);


        protected:


          /**
           * Returns a pointer to the cache where the session
           * data is stored.
           */
          virtual granada::cache::CacheHandler* cache(){
            return nullptr;
          }


          /**
           * Returns the key to identify the session data
           * in the cache.
           */
          virtual const std::string session_data_hash(){
            return cache_namespaces::session_data + token_;
          };

      };


      class StorageSessionRoles : public SessionRoles{
        public:
          StorageSessionRoles(){};
      };


      class StorageSessionHandler : public SessionHandler{
        public:
          StorageSessionHandler(){};
      };


      class StorageSessionCheckpoint : public SessionCheckpoint{
        public:
          StorageSessionCheckpoint(){};
      };

    }
  }
}
