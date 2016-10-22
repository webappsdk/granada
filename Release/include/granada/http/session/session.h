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
  * Stores session token in cookies or GET or POST json objects.
  *
  */
#pragma once
#include <memory>
#include <mutex>
#include <string>
#include "cpprest/details/basic_types.h"
#include "cpprest/json.h"
#include "cpprest/http_listener.h"
#include "granada/defaults.h"
#include "granada/functions.h"
#include "granada/util/memory.h"
#include "granada/util/application.h"
#include "granada/util/time.h"
#include "granada/util/string.h"
#include "granada/util/json.h"
#include "granada/http/parser.h"
#include "granada/crypto/nonce_generator.h"
#include "granada/cache/cache_handler.h"

namespace granada{
  namespace http{

    /**
     * Namespace for Sessions, Sessions Handlers, Session Roles and Session factories.
     */
    namespace session{

      class SessionRoles;
      class SessionHandler;
      class SessionFactory;

      /**
       * Abstract Session class that allows to manage session roles.
       * Stores session token in cookies or GET or POST json objects.
       */
      class Session
      {
        public:


          /**
           * Constructor.
           */
          Session(){};


          /**
           * Constructor.
           * Loads session.
           * Retrieves the token of the session from the HTTP request
           * and loads a session using the session handler.
           * If session does not exist or token is not found
           * a new session is created.
           * If session token is stored in cookies, a cookie will be set
           * in the HTTP response object.
           * This constructor is recommended for sessions that store token in cookie
           *
           * @param  request  Http request.
           * @param  response Http response.
           */
          Session(const web::http::http_request &request,web::http::http_response &response){};


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
          Session(const web::http::http_request &request){};


          /**
           * Constructor.
           * Loads a session with the given token using the session handler.
           * Use this loader if you have the token and you are not using cookies.
           * 
           * @param token Session token.
           */
          Session(const std::string& token){};


          /**
           * Destructor
           */
          virtual ~Session(){};


          /**
           * Set the value of the sessions, may be overriden in case we want to
           * make other actions.
           * 
           * @param token         Session token.
           * @param update_time   Session update time.
           */
          virtual void set(const std::string token,const std::time_t update_time){
            token_ = std::move(token);
            update_time_ = std::move(update_time);
          };


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
           * Updates a session, updating the session update time to now and saving it.
           * That means the session will timeout in now + timeout. It will keep
           * the session alive.
           */
          virtual void Update();


          /**
           * Closes a session deleting it.
           * And call all the close callback functions.
           */
          virtual void Close();


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
           * Returns true if the session is valid, false if it is not.
           * The base session will only check if the session is timed out, but this function
           * may be overriden if we need to add other validity factors.
           * @return true if session is valid, false if it is not.
           */
          virtual const bool IsValid();


          /**
           * Returns true if the session is a garbage session, false if it is not.
           * A garbage session is different from a not valid session. A session may not
           * be valid but we still don't want to delete it immediately. Maybe we want
           * to delete it after it has been timed out for three hours or when other
           * conditions are true.
           * This function is called by the functions that "clean" sessions from wherever
           * they are stored.
           * @return true is session is garbage, false if it is not.
           */
          virtual const bool IsGarbage();


          /**
           * Returns the number of seconds the session is valid before
           * it times out when not used.
           * @return Timeout in seconds.
           */
          virtual const long GetSessionTimeout();


          /**
           * Write session data.
           * @param key   Key or name of the data.
           * @param value Data as string.
           */
          virtual void Write(const std::string& key, const std::string& value);


          /**
           * Read session data.
           * @param  key Key or name of the data.
           * @return     Data as string.
           */
          virtual const std::string Read(const std::string& key);


          /**
           * Destroy session data with given key.
           * @param key Data key or name.
           */
          virtual void Destroy(const std::string& key);


          /**
           * Returns the session in a JSON object format.
           * @return  Session in form of JSON object.
           *          Example:@code
           *                {
           *                  "token"       : "cc9sKWG6PbhwyhP3EuBq8Fgve7ZycsRplduswcKIDwDuyrQ2jySsYG1v7gq1Vecf",
           *                  "update_time" :  "6346464"
           *                  "timeout"     :  "123456789"
           *                 }@endcode
           */
          virtual web::json::value to_json();


          /**
           * Returns the session unique token.
           * @return token
           */
          virtual const std::string& GetToken(){
            return token_;
          };


          /**
           * Sets the session unique token.
           */
          virtual void SetToken(const std::string& token){
            token_.assign(token);
          };


          /**
           * Returns the last modification time.
           * @return Last modification time.
           */
          virtual const std::time_t& GetUpdateTime(){
            return update_time_;
          };


          /**
           * Sets the last modification time.
           */
          virtual void SetUpdateTime(const std::time_t& update_time){
            update_time_ = update_time;
          };


          /**
           * Returns a pointer to the roles of a session.
           * @return Pointer to the roles of the session.
           */
          virtual granada::http::session::SessionRoles* roles(){
            return nullptr;
          };


          /**
           * Returns the pointer of Session Handler that manages the session.
           * @return Session Handler.
           */
          virtual granada::http::session::SessionHandler* session_handler(){
            return nullptr;
          };


          /**
           * Returns a pointer to the collection of functions
           * that are called when closing the session.
           * 
           * @return  Pointer to the collection of functions that are
           *          called when session is closed.
           */
          virtual granada::Functions* close_callbacks(){
            return nullptr;
          };


        protected:

          /**
           * Default token support, cookie || query || json.
           * Where the token will be stored-retrieved in the client-side.
           * This default value is taken in case "session_token_support" property is not found.
           */
          static std::vector<std::string> DEFAULT_SESSIONS_TOKEN_SUPPORT;


          /**
           * Mutex used when performing an action after 
           * checking that a session exists, for example when opening
           * a session.
           */
          static std::mutex session_exists_mtx_;


          /**
           * The name of the cookie or the key where the token value
           * is stored. This value is taken from the "session_token_label"
           * property of the server.conf file. If no value is indicated
           * the value will be taken from the "session_token_label"
           * property in defaults.dat
           * 
           */
          static std::string token_label_;


          /**
           * Where the session token is stored: cookie || query || json
           * by default. This value is taken from the "session_token_support"
           * property of the server.conf file. If no value is indicated
           * the value will be taken from the "session_token_support"
           * property in defaults.dat
           */
          static std::string application_session_token_support_;


          /**
           * Time in seconds that has to pass since the last session use until
           * the session is usable. This value is taken from the "session_timeout"
           * property of the server.conf file. If no value is indicated
           * the value will be taken from the "session_timeout"
           * property in defaults.dat
           */
          static long application_session_timeout_;


          /**
           * Number of seconds that needs to pass after a session
           * is timed out to be considered garbage.
           * in the "session_garbage_extra_timeout" property
           * If no property indicated, it will take default_numbers::session_session_garbage_extra_timeout.
           */
          static long session_garbage_extra_timeout_;


          /**
           * Where the session token is stored: cookie || query || json
           * for this session. It can be different from the
           * application_session_token_support_
           */
          std::string session_token_support_;


          /**
           * Session token: Unique identifier of the session.
           */
          std::string token_;


          /**
           * Last time we have used the session.
           */
          std::time_t update_time_;


          /**
           * Method that loads the session properties: token label,
           * token support, session timout...
           */
          virtual void LoadProperties();


          /**
           * Loads session.
           * Retrieves the token of the session from the HTTP request
           * and loads a session using the session handler.
           * If session does not exist or token is not found
           * a new session is created.
           * This loader is recommended for sessions that store token in cookie
           *
           * @param  request  Http request.
           * @param  response Http response.
           * @return          True if session has been retrieved or created successfuly.
           */
          virtual const bool LoadSession(const web::http::http_request &request,web::http::http_response &response);


          /**
           * Loads session.
           * Retrieves the token of the session from the HTTP request
           * and loads a session using the session handler.
           * If session does not exist or token is not found
           * a new session is created.
           * This loader is recommended for sessions that use get and post values.
           * 
           * @param  request  Http request.
           * @return          True if session has been retrieved or created successfuly.
           */
          virtual const bool LoadSession(const web::http::http_request &request);


          /**
           * Loads a session with the given token using the session handler.
           * Use this loader if you have the token and you are not using cookies.
           * 
           * @param token Session token.
           * @return      True if session has been retrieved successfuly.
           */
          virtual const bool LoadSession(const std::string& token);


          /**
           * Returns the key to identify the session data
           * in the cache.
           */
          virtual const std::string session_data_hash(){
            return cache_namespaces::session_data + token_;
          };


          /**
           * Returns the label of the token. It is used for example
           * when a token has to be extracted from a cookie.
           * @return  Label of the token.
           */
          virtual const std::string& token_label(){
            return Session::token_label_;
          };


          /**
           * Returns where the session token is stored: cookie || query || json
           * by default.
           * @return  Where the session token is stored: cookie || query || json
           *          by default.
           */
          virtual const std::string& application_session_token_support(){
            return Session::application_session_token_support_;
          };


          /**
           * Returns the time in seconds that has to pass since the last session use until
           * the session is usable.
           * @return  The time in seconds that has to pass since the last session use until
           *          the session is usable.
           */
          virtual const long& application_session_timeout(){
            return Session::application_session_timeout_;
          };


          /**
           * Returns the number of seconds that needs to pass after a session
           * is timed out to be considered garbage.
           */
          virtual const long& session_garbage_extra_timeout(){
            return Session::session_garbage_extra_timeout_;
          }

      };


      /**
       * Class for managing session roles.
       * Roles are used to manage user permissions,
       * for example letting or not the user to access some data. 
       */
      class SessionRoles
      {
        public:

          /**
           * Constructor
           */
          SessionRoles(){};


          /**
           * Constructor
           * 
           * @param session Pointer to session owner of the roles.
           */
          SessionRoles(granada::http::session::Session* session){
            session_ = session;
          };


          /**
           * Sets the session owner of the roles.
           * 
           * @param session Pointer to session owner of the roles.
           */
          virtual void SetSession(granada::http::session::Session* session){
            session_ = session;
          }


          /**
           * Check if the role with the given name is stored.
           * Returns true if the role is found or false if it does not.
           * @param  role_name Name of the role to check.
           * @return           true | false
           */
          virtual const bool Is(const std::string& role_name);


          /**
           * Add a new role if it has not already been added.
           * @param  role_name Name of the role.
           * @return           True if role added correctly, false if the role
           *                   has not been added, for example because the role
           *                   is already added.
           */
          virtual const bool Add(const std::string& role_name);


          /**
           * Remove a role.
           * @param role_name Name of the role.
           */
          virtual void Remove(const std::string& role_name);


          /**
           * Remove all roles.
           */
          virtual void RemoveAll();


          /**
           * Set a role property, it has to be a string.
           * @param role_name Role name
           * @param key       Key or name of the property.
           * @param value     Value of the property.
           */
          virtual void SetProperty(const std::string& role_name, const std::string& key, const std::string& value);


          /**
           * Get a role property, returns a string.
           * @param  role_name Role name.
           * @param  key       Key or name of the property.
           * @return           Value of the property (string).
           */
          virtual const std::string GetProperty(const std::string& role_name, const std::string& key);


          /**
           * Remove a role property.
           * @param role_name Role name.
           * @param key       Key or name of the property.
           */
          virtual void DestroyProperty(const std::string& role_name, const std::string& key);


        protected:

          /**
           * Pointer of the session, owner of the roles.
           */
          granada::http::session::Session* session_;


          /**
           * Returns the key to access a role data.
           * 
           * @param role_name Name of the role.
           * @return          Returns the key to access a role data.
           */
          virtual const std::string session_roles_hash(const std::string& role_name){
            return cache_namespaces::session_roles + session_->GetToken() + ":" + role_name;
          };
      };



      /**
       * Abstract class for managing sessions life.
       */
      class SessionHandler
      {
        public:

          /**
           * Constructor
           */
          SessionHandler(){};


          /**
           * Check if session exists wherever sessions are stored.
           * Returns true if session exists and false if it does not.
           * @param  token Token of the session to check.
           * @return       true if session exists and false if it does not.
           */
          virtual const bool SessionExists(const std::string& token);


          /**
           * Generate a new unique token.
           * @return Generated Token.
           */
          virtual const std::string GenerateToken();


          /**
           * Search if the session exists wherever the sessions are stored,
           * retrieves the value and assign it to the virgin session.
           * @param token  Token of the session to search.
           * @param virgin Pointer of the virgin session.
           */
          virtual void LoadSession(const std::string& token, granada::http::session::Session* virgin);


          /**
           * Insert or replace a session wherever the sessions are stored.
           * @param session Pointer to Session to save.
           */
          virtual void SaveSession(granada::http::session::Session* session);


          /**
           * Remove session from wherever the sessions are stored.
           * @param session Session to remove.
           */
          virtual void DeleteSession(granada::http::session::Session* session);


          /**
           * Remove garbage sessions from wherever sessions are stored.
           * It can be called from an application control panel, or better
           * called every n seconds, hours or days.
           */
          virtual void CleanSessions();


          /**
           * Clean session function but optionaly recursive each
           * x seconds, with x equal to the value given in the
           * "session_clean_frequency" property.
           * @param recursive true if recursive, false if not.
           */
          virtual void CleanSessions(bool recursive);


          /**
           * Returns a pointer to the cache handler used to store the sessions data.
           * @return Pointer to the cache handler used to store the sessions data.
           */
          virtual granada::cache::CacheHandler* cache(){
            return nullptr;
          }


        protected:


          /**
           * Token length. It will be set on LoadProperties(), if not found, it will take
           * the value of nonce_lengths::Session_token.
           */
          static int token_length_;


          /**
           * Frequency in seconds the CleanSessions function will be executed.
           * it will be set on LoadProperties(), if not found, it will take the
           * value of default_numbers::session_clean_sessions_frequency.
           */
          static double clean_sessions_frequency_;


          /**
           * Loads properties needed, like clean session frequency.
           */
          virtual void LoadProperties();


          /**
           * Returns a pointer to a generator of random alphanumeric strings.
           * Used to generate sessions' tokens.
           * @return  Pointer to a generator of random alphanumeric strings.
           */
          virtual granada::crypto::NonceGenerator* nonce_generator(){
            return nullptr;
          }


          /**
           * Returns a pointer to a session factory. It Aallows
           * to have a unique point for
           * checking and setting sessions. Used to create a new
           * session if it does not exist or if it is timed out.
           * @return  Pointer to a session factory
           */
          virtual granada::http::session::SessionFactory* factory(){
            return nullptr;
          }


          /**
           * Returns the lenght that the sesions' tokens will have.
           * @return  Lenght that the sesions' tokens will have.
           */
          virtual int& token_length(){
            return SessionHandler::token_length_;
          }


          /**
           * Returns the frequency in seconds the CleanSessions
           * function will be executed.
           * @return  The frequency in seconds the CleanSessions
           *          function will be executed.
           */
          virtual double& clean_sessions_frequency(){
            return SessionHandler::clean_sessions_frequency_;
          }


          /**
           * Returns the key used to identify the session data in the cache.
           * 
           * @param token Session token.
           * @return      Key used to identify the session data in the cache.
           */
          virtual const std::string session_value_hash(const std::string& token){
            return cache_namespaces::session_value + token;
          }
      };



      /**
       * Abstract class, checks a session.
       * Session factory. Allows to have a unique point for
       * checking and setting sessions. Used to create a new
       * session if it does not exist or if it is timed out.
       */
      class SessionFactory
      {

        public:

          /**
           * Checks if session is open / valid.
           * Can be used in case we want to open a session in case it does not exist,
           * or in case it is timed out.
           */
          virtual std::unique_ptr<granada::http::session::Session> Session_unique_ptr(){
            return granada::util::memory::make_unique<granada::http::session::Session>();
          };


          /**
           * Checks if session is open / valid.
           * Can be used in case we want to open a session in case it does not exist,
           * or in case it is timed out.
           * @param request   HTTP request.
           * @param response  HTTP response.
           */
          virtual std::unique_ptr<granada::http::session::Session> Session_unique_ptr(const web::http::http_request &request,web::http::http_response &response){
            return granada::util::memory::make_unique<granada::http::session::Session>(request,response);
          };


          /**
           * Checks if session is open / valid.
           * Can be used in case we want to open a session in case it does not exist,
           * or in case it is timed out.
           * @param request   HTTP request.
           */
          virtual std::unique_ptr<granada::http::session::Session> Session_unique_ptr(const web::http::http_request &request){
            return granada::util::memory::make_unique<granada::http::session::Session>(request);
          };


          /**
           * Checks if session is open / valid.
           * Can be used in case we want to open a session in case it does not exist,
           * or in case it is timed out.
           * @param token   Session token.
           */
          virtual std::unique_ptr<granada::http::session::Session> Session_unique_ptr(const std::string& token){
            return granada::util::memory::make_unique<granada::http::session::Session>(token);
          };

      };

    }
  }
}
