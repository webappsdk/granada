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
  * Abstract Session class that allows to manage session roles.
  * Store session token in cookies or GET or POST json objects.
  *
  */
#pragma once
#include <string>
#include "cpprest/json.h"
#include "cpprest/http_listener.h"
#include "granada/util/time.h"
#include "granada/util/string.h"
#include "granada/http/parser.h"
#include "roles.h"
#include "session_handler.h"

namespace granada{
  namespace http{
    namespace session{

      class SessionHandler;
      class Roles;
      class Session
      {
        public:


          /**
           * Constructors
           */
          Session(){};
          Session(web::http::http_request &request,web::http::http_response &response){};
          Session(web::http::http_request &request){};
          Session(const std::string& token){};

          /**
           * Set the value of the sessions, may be overriden in case we want to
           * make other actions.
           * @param session
           */
          virtual void set(granada::http::session::Session* session){
            (*this) = (*session);
          };


          /**
           * Set the value of the sessions, may be overriden in case we want to
           * make other actions.
           * @param session
           */
          virtual void set(const std::string& token,const std::time_t& update_time){
            SetToken(token);
            SetUpdateTime(update_time);
          };


          /**
           * Destructor
           */
          virtual ~Session(){};


          /**
           * Opens a new session with a unique token.
           */
          virtual void Open();


          /**
           * Opens a new session with a unique token and if session token
           * support is a cookie it stores the token value in a cookie.
           * @param response Response to store the cookie with the session token.
           */
          virtual void Open(web::http::http_response &response);


          /**
           * Closes a session deleting it.
           */
          virtual void Close();


          /**
           * Updates a session, updating the session update time to now and saving it.
           * That means the session will timeout in now + timeout. It will keep
           * the session alive.
           */
          virtual void Update();


          /**
           * Returns true if the session is valid, false if it is not.
           * The base session will only check if the session is timed out, but this function
           * may be overriden if we need to add other validity factors.
           * @return true if session is valid, false if it is not.
           */
          virtual const bool IsValid();


          /**
           * Returns true if the session is a garbage session, false if it is not.
           * This function is called by the functions that "clean" sessions from wherever
           * they are stored.
           * A garbage session is different from a not valid session. A session may not
           * be valid but we still don't want to delete it immediately. Maybe we want
           * to delete it after it has been timed out for three hours, so the function
           * will only be a garbage session after it has been timed out for three hours.
           * @return true is session is garbage, false if it is not.
           */
          virtual const bool IsGarbage();


          /**
           * Returns the roles of a session.
           * @return The roles of the session.
           */
          virtual std::shared_ptr<granada::http::session::Roles> roles(){ return std::shared_ptr<granada::http::session::Roles>(nullptr); };


          /**
           * Returns the session unique token.
           * @return token
           */
          const std::string& GetToken(){ return token_; };


          /**
           * Sets the session unique token.
           */
          void SetToken(const std::string& token){ token_.assign(token); };


          /**
           * Returns the last modification time.
           * @return Last modification time.
           */
          const std::time_t& GetUpdateTime(){ return update_time_; };


          /**
           * Sets the last modification time.
           */
          void SetUpdateTime(const std::time_t& update_time){ update_time_ = update_time; };

        protected:


          /**
           * Default token name, the cookie name containing the token or, in case of
           * using POST or GET, the key name of the token value.
           * This default value is taken in case "session_token_label" property is not found.
           */
          static std::string DEFAULT_TOKEN_LABEL;


          /**
           * Default token support, cookie || query || json.
           * Where the token will be stored-retrieved in the client-side.
           * This default value is taken in case "session_token_support" property is not found.
           */
          static std::vector<std::string> DEFAULT_SESSIONS_TOKEN_SUPPORT;


          /**
           * Default session timeout seconds, by default one day = 86400 seconds.
           * This default value is taken in case "session_timeout" property is not found.
           */
          static long DEFAULT_SESSION_TIMEOUT;


          /**
           * Session token: Unique identifier of the session.
           */
          std::string token_;


          /**
           * Last time we have used the session.
           */
          std::time_t update_time_;


          /**
           * The name of the cookie or the key where the token value
           * is stored.
           */
          std::string token_label_;


          /**
           * Where the session token is stored: cookie || query || json
           */
          std::string session_token_support_;


          /**
           * Time in seconds that has to pass since the last session use until
           * the session is usable.
           */
          long session_timeout_;

          /**
           * Returns the pointer of the hanlder of the sessions lifetime,
           * and where all the application sessions are stored.
           */
          virtual granada::http::session::SessionHandler* session_handler(){ return nullptr; };


          /**
           * Load session.
           * Retrieves the token of the session and loads a session
           * using the session handler. If session does not exist or
           * token is not found a new session is created.
           *
           * Use the first loader for sessions that store token in cookie
           * Use second loader for sessions that use get and post values.
           * Use third if you have the token and you are not using cookies.
           *
           *
           *
           * @param  request  Http request.
           * @param  response Http response.
           * @return          True if session has been retrieved or created successfuly.
           */
          virtual const bool LoadSession(web::http::http_request &request,web::http::http_response &response);
          virtual const bool LoadSession(web::http::http_request &request);
          virtual const bool LoadSession(const std::string& token);


          /**
           * Returns true if the session is timed out, false if it is not.
           * @return true | false.
           */
          virtual const bool IsTimedOut();
          /**
           * Returns true if the session is timed out since the given extra seconds,
           * false if it is not.
           * @param  extra_seconds
           * @return               true | false
           */
          virtual const bool IsTimedOut(const long int& extra_seconds);

          /**
           * Method that loads the session properties: token label,
           * token support, session timout...
           */
          virtual void LoadProperties();

      };
    }
  }
}
