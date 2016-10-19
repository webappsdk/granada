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
  * Session with all its data stored in shared map cahes.
  *
  */

#pragma once
#include "session.h"
#include "granada/cache/shared_map_cache_driver.h"

namespace granada{
  namespace http{
    namespace session{

      class MapSessionHandler;

      /**
       * Session with all its data stored in shared map cahes.
       */
      class MapSession : public Session
      {
        public:

          /**
           * Constructor
           */
          MapSession();


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
          MapSession(const web::http::http_request &request,web::http::http_response &response);


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
          MapSession(const web::http::http_request &request);


          /**
           * Constructor.
           * Loads a session with the given token using the session handler.
           * Use this loader if you have the token and you are not using cookies.
           * 
           * @param token Session token.
           */
          MapSession(const std::string& token);


          /**
           * Destructor
           */
          virtual ~MapSession(){};


          /**
           * Returns a pointer to the roles of a session.
           * @return Pointer to the roles of the session.
           */
          virtual granada::http::session::SessionRoles* roles() override {
            return roles_.get();
          };


          /**
           * Returns the pointer of Session Handler that manages the session.
           * @return Session Handler.
           */
          virtual granada::http::session::SessionHandler* session_handler() override {
            return session_handler_.get();
          };


          /**
           * Returns a pointer to the collection of functions
           * that are called when closing the session.
           * 
           * @return  Pointer to the collection of functions that are
           *          called when session is closed.
           */
          virtual granada::Functions* close_callbacks() override {
            return MapSession::close_callbacks_.get();
          };


        private:


          /**
           * Once flag for properties loading.
           */
          static std::once_flag properties_flag_;


          /**
           * Manager of the roles of the session and its properties
           */
          static std::unique_ptr<granada::Functions> close_callbacks_;


          /**
           * Hanlder of the sessions lifetime, and where all the application sessions are stored.
           */
          static std::unique_ptr<granada::http::session::SessionHandler> session_handler_;


          /**
           * Manager of the roles of the session and its properties
           */
          std::unique_ptr<granada::http::session::SessionRoles> roles_;


      };



      class MapSessionRoles : public SessionRoles
      {
        public:

          /**
           * Constructor
           */
          MapSessionRoles(granada::http::session::Session* session){
            session_ = session;
          };

      };



      class MapSessionHandler : public SessionHandler
      {
        public:

          /**
           * Constructor
           * Initialize the session properties and the 
           * session cleaner once per all the MapSessions.
           */
          MapSessionHandler(){
            std::call_once(MapSessionHandler::properties_flag_, [this](){
              this->LoadProperties();
            });

            // thread for cleaning the sessions.
            std::call_once(MapSessionHandler::clean_sessions_flag_, [this](){
              if (clean_sessions_frequency()>-1){
                pplx::create_task([this]{
                  this->CleanSessions(true);
                });
              }
            });
            
          };



          /**
           * Returns a pointer to the cache used to store the sessions' values.
           * @return  Pointer to the cache used to store the sessions' values.
           */
          virtual granada::cache::CacheHandler* cache() override {
            return MapSessionHandler::cache_.get();
          }

        protected:


          /**
           * Returns a pointer to a nonce string generator,
           * for generating unique strings tokens.
           * @return  Pointer to a nonce string generator,
           *          for generating unique strings tokens.
           */
          virtual granada::crypto::NonceGenerator* nonce_generator() override {
            return MapSessionHandler::nonce_generator_.get();
          }


          /**
           * Returns a Checkpoint Session pointer used to test sessions
           * status without knowing their type.
           * @return  Checkpoint Session pointer used to test sessions
           *          status without knowing their type.
           */
          virtual granada::http::session::SessionFactory* factory() override {
            return MapSessionHandler::factory_.get();
          }


        private:
          

          /**
           * Once flag for properties loading.
           */
          static std::once_flag properties_flag_;


          /**
           * Once flag for calling clean sessions function only
           * once.
           */
          static std::once_flag clean_sessions_flag_;


          /**
           * Pointer to the cache used to store the sessions' values.
           */
          static std::unique_ptr<granada::cache::CacheHandler> cache_;


          /**
           * Nonce string generator, for generating unique strings tokens.
           * Generate a nonce string containing random alphanumeric characters (A-Za-z0-9).
           */
          static std::unique_ptr<granada::crypto::NonceGenerator> nonce_generator_;


          /**
           * Checkpoint Session pointer used to test sessions status without knowing
           * their type.
           */
          static std::unique_ptr<granada::http::session::SessionFactory> factory_;

      };


      class MapSessionFactory : public SessionFactory{
        public:


          virtual std::unique_ptr<granada::http::session::Session> Session_unique_ptr() override {
            return granada::util::memory::make_unique<granada::http::session::MapSession>();
          };

          virtual std::unique_ptr<granada::http::session::Session> Session_unique_ptr(const web::http::http_request &request,web::http::http_response &response) override {
            return granada::util::memory::make_unique<granada::http::session::MapSession>(request,response);
          };

          virtual std::unique_ptr<granada::http::session::Session> Session_unique_ptr(const web::http::http_request &request) override {
            return granada::util::memory::make_unique<granada::http::session::MapSession>(request);
          };

          virtual std::unique_ptr<granada::http::session::Session> Session_unique_ptr(const std::string& token) override {
            return granada::util::memory::make_unique<granada::http::session::MapSession>(token);
          };
      };

    }
  }
}
