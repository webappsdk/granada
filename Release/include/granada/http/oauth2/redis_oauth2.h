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
#include "granada/cache/redis_cache_driver.h"
#include "granada/http/oauth2/oauth2.h"
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
            std::call_once(RedisOAuth2Client::properties_flag_, [this](){
              this->LoadProperties();
            });
          };


          /**
           * Constructor
           * Initialize nonce generator, load properties and load client with the given id.
           */
          RedisOAuth2Client(const std::string& id){
            std::call_once(RedisOAuth2Client::properties_flag_, [this](){
              this->LoadProperties();
            });
            id_ = id;
            Load();
          };

          // override
          virtual granada::cache::CacheHandler* cache() override {
            return cache_.get();
          };

          // override
          virtual granada::crypto::Cryptograph* cryptograph() override {
            return cryptograph_.get();
          };

          // override
          virtual granada::crypto::NonceGenerator* nonce_generator() override {
            return n_generator_.get();
          };


        private:

          /**
           * Once flag for loading properties only once.
           */
          static std::once_flag properties_flag_;


          /**
           * Cache to insert, modify and delete client data.
           */
          static std::unique_ptr<granada::cache::CacheHandler> cache_;


          /**
           * Cryptograph to encrypt and decrypt client credentials.
           */
          static std::unique_ptr<granada::crypto::Cryptograph> cryptograph_;


          /**
           * Nonce string generator, for generating unique strings tokens.
           * Generate a nonce string containing random alphanumeric characters (A-Za-z0-9).
           */
          static std::unique_ptr<granada::crypto::NonceGenerator> n_generator_;
      };


      class RedisOAuth2User : public OAuth2User{
        public:
          /**
           * Constructor
           * Initialize nonce generator and load properties.
           */
          RedisOAuth2User(){
            std::call_once(RedisOAuth2User::properties_flag_, [this](){
              this->LoadProperties();
            });
          };


          /**
           * Constructor
           * Initialize nonce generator, load properties and load user with the given username.
           */
          RedisOAuth2User(const std::string& username){
            std::call_once(RedisOAuth2User::properties_flag_, [this](){
              this->LoadProperties();
            });
            username_ = username;
            Load();
          };

          // override
          virtual granada::cache::CacheHandler* cache() override {
            return cache_.get();
          };

          // override
          virtual granada::crypto::Cryptograph* cryptograph() override {
            return cryptograph_.get();
          };

          // override
          virtual granada::crypto::NonceGenerator* nonce_generator() override {
            return n_generator_.get();
          };


        private:

          /**
           * Once flag for loading properties only once.
           */
          static std::once_flag properties_flag_;


          /**
           * Cache to insert, modify and delete client data.
           */
          static std::unique_ptr<granada::cache::CacheHandler> cache_;


          /**
           * Cryptograph to encrypt and decrypt client credentials.
           */
          static std::unique_ptr<granada::crypto::Cryptograph> cryptograph_;


          /**
           * Nonce string generator, for generating unique strings tokens.
           * Generate a nonce string containing random alphanumeric characters (A-Za-z0-9).
           */
          static std::unique_ptr<granada::crypto::NonceGenerator> n_generator_;
      };


      class RedisOAuth2Code : public OAuth2Code{
        public:
          /**
           * Constructor
           * Initialize nonce generator and load properties.
           */
          RedisOAuth2Code(){
            std::call_once(RedisOAuth2Code::properties_flag_, [this](){
              this->LoadProperties();
            });
          };


          /**
           * Constructor
           * Initialize nonce generator, load properties and load code with the given code.
           */
          RedisOAuth2Code(const std::string& code){
            std::call_once(RedisOAuth2Code::properties_flag_, [this](){
              this->LoadProperties();
            });
            code_ = code;
            Load();
          };

          // override
          virtual granada::cache::CacheHandler* cache() override {
            return cache_.get();
          };

          // override
          virtual granada::crypto::Cryptograph* cryptograph() override {
            return cryptograph_.get();
          };

          // override
          virtual granada::crypto::NonceGenerator* nonce_generator() override {
            return n_generator_.get();
          };


        private:

          /**
           * Once flag for loading properties only once.
           */
          static std::once_flag properties_flag_;


          /**
           * Cache to insert, modify and delete client data.
           */
          static std::unique_ptr<granada::cache::CacheHandler> cache_;


          /**
           * Cryptograph to encrypt and decrypt client credentials.
           */
          static std::unique_ptr<granada::crypto::Cryptograph> cryptograph_;


          /**
           * Nonce string generator, for generating unique strings tokens.
           * Generate a nonce string containing random alphanumeric characters (A-Za-z0-9).
           */
          static std::unique_ptr<granada::crypto::NonceGenerator> n_generator_;
      };


      class RedisOAuth2Authorization : public OAuth2Authorization{
        public:

          RedisOAuth2Authorization(){
            std::call_once(RedisOAuth2Authorization::properties_flag_, [this](){
              this->LoadProperties();
            });
          };


          RedisOAuth2Authorization(const granada::http::oauth2::OAuth2Parameters& oauth2_parameters,
                                    granada::http::session::SessionFactory* session_factory){
            oauth2_parameters_ = oauth2_parameters;
            session_factory_ = session_factory;
            std::call_once(RedisOAuth2Authorization::properties_flag_, [this](){
              this->LoadProperties();
            });
          };

          // override
          granada::cache::CacheHandler* cache() override {
            return cache_.get();
          };

        protected:

          virtual granada::http::oauth2::OAuth2Factory* factory() override {
            return oauth2_factory_.get();
          };


        private:

          /**
           * Once flag for loading properties only once.
           */
          static std::once_flag properties_flag_;


          /**
           * OAuth 2.0 Factory.
           * Used to instanciate OAuth 2.0 clients, users and codes.
           */
          static std::unique_ptr<granada::http::oauth2::OAuth2Factory> oauth2_factory_;


          /**
           * Cache to insert, modify and delete client data.
           */
          static std::unique_ptr<granada::cache::CacheHandler> cache_;
      };


      class RedisOAuth2Factory : public OAuth2Factory{

        public:

          virtual std::unique_ptr<granada::http::oauth2::OAuth2Client>OAuth2Client_unique_ptr(){
            return granada::util::memory::make_unique<granada::http::oauth2::RedisOAuth2Client>();
          };

          virtual std::unique_ptr<granada::http::oauth2::OAuth2Client>OAuth2Client_unique_ptr(const std::string& client_id){
            return granada::util::memory::make_unique<granada::http::oauth2::RedisOAuth2Client>(client_id);
          };

          virtual std::unique_ptr<granada::http::oauth2::OAuth2User>OAuth2User_unique_ptr(){
            return granada::util::memory::make_unique<granada::http::oauth2::RedisOAuth2User>();
          };

          virtual std::unique_ptr<granada::http::oauth2::OAuth2User>OAuth2User_unique_ptr(const std::string& username){
            return granada::util::memory::make_unique<granada::http::oauth2::RedisOAuth2User>(username);
          };

          virtual std::unique_ptr<granada::http::oauth2::OAuth2Code>OAuth2Code_unique_ptr(){
            return granada::util::memory::make_unique<granada::http::oauth2::RedisOAuth2Code>();
          };

          virtual std::unique_ptr<granada::http::oauth2::OAuth2Code>OAuth2Code_unique_ptr(const std::string& code){
            return granada::util::memory::make_unique<granada::http::oauth2::RedisOAuth2Code>(code);
          };

          virtual std::unique_ptr<granada::http::oauth2::OAuth2Authorization>OAuth2Authorization_unique_ptr(){
            return granada::util::memory::make_unique<granada::http::oauth2::RedisOAuth2Authorization>();
          };

          virtual std::unique_ptr<granada::http::oauth2::OAuth2Authorization>OAuth2Authorization_unique_ptr(const granada::http::oauth2::OAuth2Parameters& oauth2_parameters,
                                                                                                 granada::http::session::SessionFactory* session_factory){
            return granada::util::memory::make_unique<granada::http::oauth2::RedisOAuth2Authorization>(oauth2_parameters,session_factory);
          };

      };

    }

  }

}
