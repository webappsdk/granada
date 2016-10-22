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

#include "granada/http/oauth2/redis_oauth2.h"

namespace granada{

  namespace http{

    namespace oauth2{

      std::once_flag RedisOAuth2Client::properties_flag_;
      std::unique_ptr<granada::cache::CacheHandler> RedisOAuth2Client::cache_(new granada::cache::RedisCacheDriver());
      std::unique_ptr<granada::crypto::Cryptograph> RedisOAuth2Client::cryptograph_(new granada::crypto::OpensslAESCryptograph());
      std::unique_ptr<granada::crypto::NonceGenerator> RedisOAuth2Client::n_generator_(new granada::crypto::CPPRESTNonceGenerator());

      std::once_flag RedisOAuth2User::properties_flag_;
      std::unique_ptr<granada::cache::CacheHandler> RedisOAuth2User::cache_(new granada::cache::RedisCacheDriver());
      std::unique_ptr<granada::crypto::Cryptograph> RedisOAuth2User::cryptograph_(new granada::crypto::OpensslAESCryptograph());
      std::unique_ptr<granada::crypto::NonceGenerator> RedisOAuth2User::n_generator_(new granada::crypto::CPPRESTNonceGenerator());

      std::once_flag RedisOAuth2Code::properties_flag_;
      std::unique_ptr<granada::cache::CacheHandler> RedisOAuth2Code::cache_(new granada::cache::RedisCacheDriver());
      std::unique_ptr<granada::crypto::Cryptograph> RedisOAuth2Code::cryptograph_(new granada::crypto::OpensslAESCryptograph());
      std::unique_ptr<granada::crypto::NonceGenerator> RedisOAuth2Code::n_generator_(new granada::crypto::CPPRESTNonceGenerator());
      
      std::once_flag RedisOAuth2Authorization::properties_flag_;
      std::unique_ptr<granada::http::oauth2::OAuth2Factory> RedisOAuth2Authorization::oauth2_factory_(new granada::http::oauth2::RedisOAuth2Factory());
      std::unique_ptr<granada::cache::CacheHandler> RedisOAuth2Authorization::cache_(new granada::cache::RedisCacheDriver());
    }
  }
}
