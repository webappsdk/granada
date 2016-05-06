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
  * User
  * Class for creating, verifying and deleting user as well as opening sessions
  * related to the user.
  *
  */

#pragma once

#include <string>
#include <mutex>
#include <memory>
#include <sstream>
#include "granada/util/string.h"
#include "granada/cache/redis_cache_driver.h"
#include "granada/crypto/openssl_aes_cryptograph.h"
#include "granada/http/session/session.h"
#include "cpprest/http_listener.h"

namespace granada{
  class User{
    public:

      /**
       * Constructor
       */
      User(){};


      /**
       * Returns true if user with the given username exists.
       * @param  username Username
       * @return          True if user exists and false if it does not.
       */
      bool Exists(const std::string& username);


      /**
       * Create a user with given creadentials username and password if it does not
       * exists already.
       * @param  username
       * @param  password
       * @return
       */
      bool Create(const std::string& username, std::string& password);


      /**
       * Delete user with given credentials.
       * @param  username
       * @param  password
       * @return          True if user has destroyed and false if it has not.
       */
      bool Delete(const std::string& username, const std::string& password);


      /**
       * Opens a session and relate it to the user if the credentials are correct.
       * @param  username
       * @param  password
       * @param  session
       * @param  response
       * @return         True if credentials are OK and session has been opened.
       */
      bool AssignSession(const std::string& username, std::string& password, granada::http::session::Session* session, web::http::http_response response);


      /**
       * Returns the roles a session related to the user can have in form of string.
       * Example: "USER,MSG_READER,MSG_WRITER" => means the related session can
       * have USER, MSG_READER and MSG_WRITER roles.
       * @param  username
       * @return          Roles in form of string.
       */
      std::string Roles(const std::string& username);

    private:

      /**
       * Mutex for thread safety.
       */
      std::mutex mtx;


      /**
       * Cache to insert, modify and delete user data.
       */
      static std::unique_ptr<granada::cache::RedisCacheDriver> cache_;


      /**
       * Cryptograph to encrypt and decrypt user credentials.
       */
      static std::unique_ptr<granada::crypto::OpensslAESCryptograph> cryptograph_;
  };
}
