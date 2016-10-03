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
  * Manage the lifecycle of the sessions, store them in an unordered map
  * shared by all users. It is multithread safe.
  * 
  */
#pragma once
#include <thread>
#include <chrono>
#include <mutex>
#include "pplx/pplxtasks.h"
#include <memory>
#include <string>
#include <unordered_map>
#include "session_handler.h"
#include "granada/defaults.h"
#include "granada/util/application.h"
#include "granada/crypto/nonce_generator.h"

namespace granada{
  namespace http{
    namespace session{

      /**
       * Manage the lifecycle of the sessions, store them in an unordered map
       * shared by all users. It is multithread safe.
       */
      class SharedMapSessionHandler : public SessionHandler
      {
        public:

          /**
           * Constructor
           */
          SharedMapSessionHandler();


          // override
          const std::string GetProperty(const std::string& name);


          // override
          const bool SessionExists(const std::string& token);


          // override
          const std::string GenerateToken();


          // override
          void LoadSession(const std::string& token,granada::http::session::Session* virgin);


          // override
          void SaveSession(std::shared_ptr<granada::http::session::Session> session);


          // override
          void DeleteSession(granada::http::session::Session* session);

          // override
          void CleanSessions();

          /**
           * Clean session function but optionaly recursive each
           * x seconds, with x equal to the value given in the
           * "session_clean_frequency" property.
           * @param recursive true if recursive, false if not.
           */
          void CleanSessions(bool recursive);


        private:

          /**
           * Unordered map where all sessions are stored.
           */
          std::unique_ptr<std::unordered_map<std::string,std::shared_ptr<granada::http::session::Session>>> sessions_;


          /**
           * Nonce string generator, for generating unique strings tokens.
           * Generate a nonce string containing random alphanumeric characters (A-Za-z0-9).
           */
          std::unique_ptr<granada::crypto::NonceGenerator> n_generator_;


          /**
           * Token length. It will be set on LoadProperties(), if not found, it will take
           * the value of nonce_lengths::session_token.
           */
          int token_length_;


          /**
           * Frequency in seconds the CleanSessions function will be executed.
           * it will be set on LoadProperties(), if not found, it will take the
           * value of default_numbers::session_clean_sessions_frequency.
           */
          double clean_sessions_frequency_;


          /**
           * Mutex.
           */
          std::mutex mtx;


          /**
           * Loads properties needed, like clean session frequency.
           */
          void LoadProperties();
      };
    }
  }
}
