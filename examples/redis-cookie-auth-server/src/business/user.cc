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

#include "user.h"

namespace granada{

  std::unique_ptr<granada::cache::RedisCacheDriver> User::cache_ = std::unique_ptr<granada::cache::RedisCacheDriver>(new granada::cache::RedisCacheDriver());
  std::unique_ptr<granada::crypto::OpensslAESCryptograph> User::cryptograph_ = std::unique_ptr<granada::crypto::OpensslAESCryptograph>(new granada::crypto::OpensslAESCryptograph());

  bool User::Exists(const std::string& username){
    return cache_->Exists("user:value:" + username);
  }

  bool User::Create(const std::string& username, std::string& password){
    mtx.lock();
    if (Exists(username)){
      mtx.unlock();
      return false;
    }else{
      cache_->Write("user:value:" + username, "username", username);
      mtx.unlock();

      std::string key = cryptograph_->Encrypt(username,password);

      cache_->Write("user:value:" + username, "key", key);

      cache_->Write("user:value:" + username, "roles", "USER,MSG_READER,MSG_WRITER");

      std::time_t creation_time = std::time(nullptr);
      std::stringstream ss;
      ss << creation_time;
      std::string update_time_str = ss.str();
      cache_->Write("user:value:" + username, "creation_time", update_time_str);

      return true;
    }
  }

  bool User::Delete(const std::string& username, const std::string& password){
    return false;
  }

  bool User::AssignSession(const std::string& username, std::string& password, granada::http::session::Session* session, web::http::http_response response){

    std::string key = cache_->Read("user:value:" + username, "key");

    std::string decrypted_key = cryptograph_->Decrypt(key,password);

    if (decrypted_key == username){
      session->Open(response);
      return true;
    }
    return false;
  }

  std::string User::Roles(const std::string& username){
    return cache_->Read("user:value:" + username, "roles");
  }

}
