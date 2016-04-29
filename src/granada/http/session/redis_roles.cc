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

#include "granada/http/session/redis_roles.h"

namespace granada{
  namespace http{
    namespace session{

      std::unique_ptr<granada::cache::RedisCacheDriver> RedisRoles::cache_ = std::unique_ptr<granada::cache::RedisCacheDriver>(new granada::cache::RedisCacheDriver());

      RedisRoles::RedisRoles(granada::http::session::Session* session){
        session_ = session;
      }

      const bool RedisRoles::Is(const std::string& role_name){
        return cache_->Exists("session:roles:" + session_->GetToken() + ":" + role_name);
      }

      const bool RedisRoles::Add(const std::string& role_name){
        // add only if role is not already added.
        if (!Is(role_name)){
          cache_->Write("session:roles:" + session_->GetToken() + ":" + role_name, "0", "0");
          return true;
        }
        return false;
      }

      void RedisRoles::Remove(const std::string& role_name){
        cache_->Destroy("session:roles:" + session_->GetToken() + ":" + role_name);
      }

      void RedisRoles::SetProperty(const std::string& role_name, const std::string& key, const std::string& value){
        cache_->Write("session:roles:" + session_->GetToken() + ":" + role_name, key, value);
      }

      const std::string RedisRoles::GetProperty(const std::string& role_name, const std::string& key){
        return cache_->Read("session:roles:" + session_->GetToken() + ":" + role_name, key);
      }

      void RedisRoles::DestroyProperty(const std::string& role_name, const std::string& key){
        cache_->Destroy("session:roles:" + session_->GetToken() + ":" + role_name, key);
      }
    }
  }
}
