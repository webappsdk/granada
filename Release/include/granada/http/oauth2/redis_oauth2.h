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
  * Classes of entities useful for OAuth 2.0 authorization,
  * use Redis to store data.
  * Based on rfc6749 document: The OAuth 2.0 Authorization Framework
  * https://tools.ietf.org/html/rfc6749
  *
  */

#pragma once
#include "granada/http/oauth2/oauth2.h"
#include "granada/cache/redis_cache_driver.h"
#include "granada/crypto/nonce_generator.h"
#include "granada/crypto/openssl_aes_cryptograph.h"

namespace granada{

  namespace http{

    namespace oauth2{

      class RedisOAuth2Client : public OAuth2Client{
        public:
          /**
           * Constructor
           * Initialize nonce generator and load properties.
           */
          RedisOAuth2Client(){
            n_generator_ = std::shared_ptr<granada::crypto::NonceGenerator>(new granada::crypto::CPPRESTNonceGenerator());
            LoadProperties();
          };


          /**
           * Constructor
           * Initialize nonce generator, load properties and load client with the given id.
           */
          RedisOAuth2Client(const std::string& id){
            n_generator_ = std::shared_ptr<granada::crypto::NonceGenerator>(new granada::crypto::CPPRESTNonceGenerator());
            LoadProperties();
            id_ = id;
            Load();
          };

          // override
          std::shared_ptr<granada::cache::CacheHandler> cache(){
            return cache_;
          };

          // override
          std::shared_ptr<granada::crypto::Cryptograph> cryptograph(){
            return cryptograph_;
          };

          // override
          std::shared_ptr<granada::crypto::NonceGenerator> nonce_generator(){
            return n_generator_;
          };


        private:

          /**
           * Cache to insert, modify and delete client data.
           */
          static std::shared_ptr<granada::cache::CacheHandler> cache_;


          /**
           * Cryptograph to encrypt and decrypt client credentials.
           */
          static std::shared_ptr<granada::crypto::Cryptograph> cryptograph_;


          /**
           * Nonce string generator, for generating unique strings tokens.
           * Generate a nonce string containing random alphanumeric characters (A-Za-z0-9).
           */
          std::shared_ptr<granada::crypto::NonceGenerator> n_generator_;
      };


      class RedisOAuth2User : public OAuth2User{
        public:
          /**
           * Constructor
           * Initialize nonce generator and load properties.
           */
          RedisOAuth2User(){
            n_generator_ = std::shared_ptr<granada::crypto::NonceGenerator>(new granada::crypto::CPPRESTNonceGenerator());
            LoadProperties();
          };


          /**
           * Constructor
           * Initialize nonce generator, load properties and load user with the given username.
           */
          RedisOAuth2User(const std::string& username){
            n_generator_ = std::shared_ptr<granada::crypto::NonceGenerator>(new granada::crypto::CPPRESTNonceGenerator());
            LoadProperties();
            username_ = username;
            Load();
          };

          // override
          std::shared_ptr<granada::cache::CacheHandler> cache(){
            return cache_;
          };

          // override
          std::shared_ptr<granada::crypto::Cryptograph> cryptograph(){
            return cryptograph_;
          };

          // override
          std::shared_ptr<granada::crypto::NonceGenerator> nonce_generator(){
            return n_generator_;
          };


        private:

          /**
           * Cache to insert, modify and delete client data.
           */
          static std::shared_ptr<granada::cache::CacheHandler> cache_;


          /**
           * Cryptograph to encrypt and decrypt client credentials.
           */
          static std::shared_ptr<granada::crypto::Cryptograph> cryptograph_;


          /**
           * Nonce string generator, for generating unique strings tokens.
           * Generate a nonce string containing random alphanumeric characters (A-Za-z0-9).
           */
          std::shared_ptr<granada::crypto::NonceGenerator> n_generator_;
      };


      class RedisOAuth2Code : public OAuth2Code{
        public:
          /**
           * Constructor
           * Initialize nonce generator and load properties.
           */
          RedisOAuth2Code(){
            n_generator_ = std::shared_ptr<granada::crypto::NonceGenerator>(new granada::crypto::CPPRESTNonceGenerator());
            LoadProperties();
          };


          /**
           * Constructor
           * Initialize nonce generator, load properties and load code with the given code.
           */
          RedisOAuth2Code(const std::string& code){
            n_generator_ = std::shared_ptr<granada::crypto::NonceGenerator>(new granada::crypto::CPPRESTNonceGenerator());
            LoadProperties();
            code_ = code;
            Load();
          };

          // override
          std::shared_ptr<granada::cache::CacheHandler> cache(){
            return cache_;
          };

          // override
          std::shared_ptr<granada::crypto::Cryptograph> cryptograph(){
            return cryptograph_;
          };

          // override
          std::shared_ptr<granada::crypto::NonceGenerator> nonce_generator(){
            return n_generator_;
          };


        private:

          /**
           * Cache to insert, modify and delete client data.
           */
          static std::shared_ptr<granada::cache::CacheHandler> cache_;


          /**
           * Cryptograph to encrypt and decrypt client credentials.
           */
          static std::shared_ptr<granada::crypto::Cryptograph> cryptograph_;


          /**
           * Nonce string generator, for generating unique strings tokens.
           * Generate a nonce string containing random alphanumeric characters (A-Za-z0-9).
           */
          std::shared_ptr<granada::crypto::NonceGenerator> n_generator_;
      };


      class RedisOAuth2Authorization : public OAuth2Authorization{
        public:

          RedisOAuth2Authorization(){
            LoadProperties();
          };


          RedisOAuth2Authorization(const granada::http::oauth2::OAuth2Parameters& oauth2_parameters,
                                   std::shared_ptr<granada::http::session::SessionCheckpoint>& session_checkpoint,
                                   std::shared_ptr<granada::http::oauth2::OAuth2Factory>& oauth2_factory){
            oauth2_parameters_ = oauth2_parameters;
            session_checkpoint_ = session_checkpoint;
            oauth2_factory_ = oauth2_factory;
            LoadProperties();
          };

          // override
          std::shared_ptr<granada::cache::CacheHandler> cache(){
            return cache_;
          };


        private:

          /**
           * Cache to insert, modify and delete client data.
           */
          static std::shared_ptr<granada::cache::CacheHandler> cache_;
      };


      class RedisOAuth2Factory : public OAuth2Factory{

        public:

          // override
          std::shared_ptr<granada::http::oauth2::OAuth2Client>OAuth2Client(){
            return std::shared_ptr<granada::http::oauth2::OAuth2Client>(new granada::http::oauth2::RedisOAuth2Client());
          };

          // override
          std::shared_ptr<granada::http::oauth2::OAuth2Client>OAuth2Client(const std::string& client_id){
            return std::shared_ptr<granada::http::oauth2::OAuth2Client>(new granada::http::oauth2::RedisOAuth2Client(client_id));
          };

          // override
          std::shared_ptr<granada::http::oauth2::OAuth2User>OAuth2User(){
            return std::shared_ptr<granada::http::oauth2::OAuth2User>(new granada::http::oauth2::RedisOAuth2User());
          };

          // override
          std::shared_ptr<granada::http::oauth2::OAuth2User>OAuth2User(const std::string& client_id){
            return std::shared_ptr<granada::http::oauth2::OAuth2User>(new granada::http::oauth2::RedisOAuth2User(client_id));
          };

          // override
          std::shared_ptr<granada::http::oauth2::OAuth2Code>OAuth2Code(){
            return std::shared_ptr<granada::http::oauth2::OAuth2Code>(new granada::http::oauth2::RedisOAuth2Code());
          };

          // override
          std::shared_ptr<granada::http::oauth2::OAuth2Code>OAuth2Code(const std::string& code){
            return std::shared_ptr<granada::http::oauth2::OAuth2Code>(new granada::http::oauth2::RedisOAuth2Code(code));
          };

          // override
          std::shared_ptr<granada::http::oauth2::OAuth2Authorization>OAuth2Authorization(){
            return std::shared_ptr<granada::http::oauth2::OAuth2Authorization>(new granada::http::oauth2::RedisOAuth2Authorization());
          };

          // override
          std::shared_ptr<granada::http::oauth2::OAuth2Authorization>OAuth2Authorization(const granada::http::oauth2::OAuth2Parameters& oauth2_parameters,
                                                                                         std::shared_ptr<granada::http::session::SessionCheckpoint>& session_checkpoint,
                                                                                         std::shared_ptr<granada::http::oauth2::OAuth2Factory>& oauth2_factory){
            return std::shared_ptr<granada::http::oauth2::OAuth2Authorization>(new granada::http::oauth2::RedisOAuth2Authorization(oauth2_parameters,session_checkpoint,oauth2_factory));
          };

      };

    }

  }

}
