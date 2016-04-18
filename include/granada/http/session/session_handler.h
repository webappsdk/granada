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
  * Interface, manage the session life.
  *
  */
#pragma once
#include <string>
#include "granada/http/session/session.h"

namespace granada{
  namespace http{
    namespace session{

      class Session;
      class SessionHandler
      {
        public:

          /**
           * Constructor
           */
          SessionHandler(){};


          /**
           * Destructor
           */
          ~SessionHandler(){};


          /**
           * Get a session property from wherever properties are stored.
           * @param  name Name of the property.
           * @return      Value of the property.
           */
          virtual std::string GetProperty(const std::string& name){ return std::string(); };


          /**
           * Check if session exists wherever sessions are stored.
           * Returns true if session exists and false if it does not.
           * @param  token Token of the session to check.
           * @return       true if session exists and false if it does not.
           */
          virtual bool SessionExists(const std::string& token){ return true; };


          /**
           * Generate a new unique token.
           * @return Generated Token.
           */
          virtual std::string GenerateToken(){ return std::string(); };


          /**
           * Search if the session exists wherever the sessions are stored,
           * retrieves the value and assign it to the virgin session.
           * @param token  Token of the session to search.
           * @param virgin Pointer of the virgin session.
           */
          virtual void LoadSession(const std::string& token,granada::http::session::Session* virgin){};


          /**
           * Insert or replace a session wherever the sessions are stored.
           * @param session Session to save.
           */
          virtual void SaveSession(std::shared_ptr<granada::http::session::Session> session){};


          /**
           * Remove session from wherever the sessions are stored.
           * @param session Session to remove.
           */
          virtual void DeleteSession(granada::http::session::Session* session){};


          /**
           * Remove garbage sessions from wherever sessions are stored.
           * It can be called from an application control panel, or better
           * called every n seconds, hours or days.
           */
          virtual void CleanSessions(){};
      };
    }
  }
}
