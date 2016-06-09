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
  * Abstract class, checks a session.
  * Allows to have a unique point for checking and setting sessions.
  * Can be used to create a new session if it does not exist.
  *
  */
#pragma once
#include <memory>
#include "cpprest/http_listener.h"
#include "granada/http/session/session.h"

namespace granada{
  namespace http{
    namespace session{
      class Checkpoint
      {
        public:

          /**
           * Constructors
           */
          Checkpoint(){};


          /**
           * Checks if session is open / valid.
           * Can be used in case we want to open a session in case it does not exist,
           * or in case it is timed out.
           */
          virtual std::shared_ptr<granada::http::session::Session> check(){
            return std::shared_ptr<granada::http::session::Session>(new granada::http::session::Session());
          };


          /**
           * Checks if session is open / valid.
           * Can be used in case we want to open a session in case it does not exist,
           * or in case it is timed out.
           * @param request   HTTP request.
           * @param response  HTTP response.
           */
          virtual std::shared_ptr<granada::http::session::Session> check(web::http::http_request &request,web::http::http_response &response){
            return std::shared_ptr<granada::http::session::Session>(new granada::http::session::Session(request,response));
          };


          /**
           * Checks if session is open / valid.
           * Can be used in case we want to open a session in case it does not exist,
           * or in case it is timed out.
           * @param request   HTTP request.
           */
          virtual std::shared_ptr<granada::http::session::Session> check(web::http::http_request &request){
            return std::shared_ptr<granada::http::session::Session>(new granada::http::session::Session(request));
          };


          /**
           * Checks if session is open / valid.
           * Can be used in case we want to open a session in case it does not exist,
           * or in case it is timed out.
           * @param token   Session token.
           */
          virtual std::shared_ptr<granada::http::session::Session> check(const std::string& token){
            return std::shared_ptr<granada::http::session::Session>(new granada::http::session::Session(token));
          };

      };
    }
  }
}
