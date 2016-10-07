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
#include "storage_session.h"
#include "granada/cache/shared_map_cache_driver.h"

namespace granada{
  namespace http{
    namespace session{

      class MapStorageSessionHandler;

      class MapStorageSession : public StorageSession
      {
        public:

          /**
           * Constructor
           */
          MapStorageSession();


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
          MapStorageSession(const web::http::http_request &request,web::http::http_response &response);


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
          MapStorageSession(const web::http::http_request &request);


          /**
           * Constructor.
           * Loads a session with the given token using the session handler.
           * Use this loader if you have the token and you are not using cookies.
           * 
           * @param token Session token.
           */
          MapStorageSession(const std::string& token);



          /**
           * Returns a pointer to the roles of a session.
           * @return Pointer to the roles of the session.
           */
          virtual granada::http::session::SessionRoles* roles() override {
            return roles_.get();
          };


          /**
           * Returns a pointer to the collection of functions
           * that are called when closing the session.
           * 
           * @return  Pointer to the collection of functions that are
           *          called when session is closed.
           */
          virtual granada::Functions* close_callbacks() override {
            return MapStorageSession::close_callbacks_.get();
          };


        protected:


          /**
           * Once flag for properties loading.
           */
          static std::once_flag properties_flag_;


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
           * Hanlder of the sessions lifetime, and where all the application sessions are stored.
           */
          static std::unique_ptr<granada::http::session::SessionHandler> session_handler_;


          /**
           * Manager of the roles of the session and its properties
           */
          std::unique_ptr<granada::http::session::SessionRoles> roles_;


          /**
           * Returns the pointer of Session Handler that manages the session.
           * @return Session Handler.
           */
          virtual granada::http::session::SessionHandler* session_handler() override {
            return session_handler_.get();
          };


          /**
           * Returns a pointer to the manager of the storage, and contains
           * the data stored.
           * @return Pointer to a CacheHandler.
           */
          virtual granada::cache::CacheHandler* cache(){
            return MapStorageSession::cache_.get();
          }

      };



      class MapStorageSessionRoles : public StorageSessionRoles
      {
        public:

          /**
           * Constructor
           */
          MapStorageSessionRoles(granada::http::session::Session* session){
            session_ = session;
          };


        protected:

          /**
           * Manager of the storage, and contains
           * the data stored.
           */
          static std::unique_ptr<granada::cache::CacheHandler> cache_;


          /**
           * Returns a pointer to the cache handler used to store the roles' data.
           * @return Pointer to the cache handler used to store the roles' data.
           */
          virtual granada::cache::CacheHandler* cache() override {
            return MapStorageSessionRoles::cache_.get();
          };

      };



      class MapStorageSessionHandler : public StorageSessionHandler
      {
        public:

          /**
           * Constructor
           * Initialize the session properties and the 
           * session cleaner once per all the MapStorageSessions.
           */
          MapStorageSessionHandler(){
            std::call_once(MapStorageSessionHandler::properties_flag_, [this](){
              this->LoadProperties();
            });

            // thread for cleaning the sessions.
            if (clean_sessions_frequency()>-1){
              pplx::create_task([this]{
                this->CleanSessions(true);
              });
            }
          };


        protected:


          /**
           * Once flag for properties loading.
           */
          static std::once_flag properties_flag_;


          /**
           * Map where all sessions are stored.
           */
          static std::unique_ptr<granada::cache::CacheHandler> cache_;


          /**
           * Nonce string generator, for generating unique strings tokens.
           * Generate a nonce string containing random alphanumeric characters (A-Za-z0-9).
           */
          static std::unique_ptr<granada::crypto::NonceGenerator> nonce_generator_;


          /**
           * Session pointer used to test sessions status without knowing
           * their type.
           */
          static std::unique_ptr<granada::http::session::SessionCheckpoint> checkpoint_;


          /**
           * Returns a pointer to the cache used to store the sessions' values.
           * @return  Pointer to the cache used to store the sessions' values.
           */
          virtual granada::cache::CacheHandler* cache() override {
            return MapStorageSessionHandler::cache_.get();
          }


          /**
           * Returns a pointer to a nonce string generator,
           * for generating unique strings tokens.
           * @return  Pointer to a nonce string generator,
           *          for generating unique strings tokens.
           */
          virtual granada::crypto::NonceGenerator* nonce_generator() override {
            return MapStorageSessionHandler::nonce_generator_.get();
          }


          /**
           * Returns a Checkpoint Session pointer used to test sessions
           * status without knowing their type.
           * @return  Checkpoint Session pointer used to test sessions
           *          status without knowing their type.
           */
          virtual granada::http::session::SessionCheckpoint* checkpoint() override {
            return MapStorageSessionHandler::checkpoint_.get();
          }
      };



      class MapStorageSessionCheckpoint : public StorageSessionCheckpoint
      {
        public:

          /**
           * Constructor
           */
          MapStorageSessionCheckpoint(){};

          virtual std::shared_ptr<granada::http::session::Session> check() override {
            return std::shared_ptr<granada::http::session::Session>(new granada::http::session::MapStorageSession());
          };

          virtual std::shared_ptr<granada::http::session::Session> check(const web::http::http_request &request,web::http::http_response &response) override {
            return std::shared_ptr<granada::http::session::Session>(new granada::http::session::MapStorageSession(request,response));
          };

          virtual std::shared_ptr<granada::http::session::Session> check(const web::http::http_request &request) override {
            return std::shared_ptr<granada::http::session::Session>(new granada::http::session::MapStorageSession(request));
          };

          virtual std::shared_ptr<granada::http::session::Session> check(const std::string& token) override {
            return std::shared_ptr<granada::http::session::Session>(new granada::http::session::MapStorageSession(token));
          };

      };


    }
  }
}
