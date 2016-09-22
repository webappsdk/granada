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
  * Simplest session, only alow to manage user roles.
  *
  */

#pragma once
#include "session.h"
#include "granada/defaults.h"
#include "map_roles.h"
#include "shared_map_session_handler.h"

namespace granada{
  namespace http{
    namespace session{

      /**
       * Simplest session, only alow to manage user roles.
       */
      class SimpleSession : public Session
      {
        public:

          /**
           * Constructor.
           */
          SimpleSession();


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
          SimpleSession(web::http::http_request &request,web::http::http_response &response);


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
          SimpleSession(web::http::http_request &request);


          /**
           * Constructor.
           * Loads a session with the given token using the session handler.
           * Use this loader if you have the token and you are not using cookies.
           * 
           * @param token Session token.
           */
          SimpleSession(const std::string& token);


          /**
           * Set the value of the sessions, may be overriden in case we want to
           * make other actions.
           * @param session
           */
          void set(granada::http::session::Session* session){
            (*this) = (*((granada::http::session::SimpleSession*)session));
            roles()->SetSession(this);
          };


          /**
           * Updates a session, updating the session update time to now and saving it.
           * That means the session will timeout in now + timeout. It will keep
           * the session alive.
           */
          void Update();


          /**
           * Returns the roles of a session.
           * @return The roles of the session.
           */
          std::shared_ptr<granada::http::session::Roles> roles(){ return roles_; };


          /**
           * Returns a pointer to the collection of functions
           * that are called when closing the session.
           * 
           * @return  Pointer to the collection of functions that are
           *          called when session is closed.
           */
          granada::Functions* close_callbacks(){ return close_callbacks_.get(); };


        protected:


          /**
           * Returns the pointer of Session Handler that manages the session.
           * @return Session Handler.
           */
          granada::http::session::SessionHandler* session_handler(){ return session_handler_.get(); };


          /**
           * Hanlder of the sessions lifetime, and where all the application sessions are stored.
           */
          static std::unique_ptr<granada::http::session::SessionHandler> session_handler_;


          /**
           * Loads the properties as session_clean_extra_timeout_.
           */
          void LoadProperties();


          /**
           * Pointer to the session roles manager and its properties.
           */
          std::shared_ptr<granada::http::session::Roles> roles_;


          /**
           * Pointer to the collection of functions that are
           * called when session is closed.
           */
          static std::unique_ptr<granada::Functions> close_callbacks_;


          /**
           * Used for determining if the session is garbage.
           * In case the session is timed out since x seconds indicated
           * in the "session_clean_extra_timeout" property
           * If no property indicated, it will take default_numbers::session_session_clean_extra_timeout.
           */
          long session_clean_extra_timeout_;

      };
    }
  }
}
