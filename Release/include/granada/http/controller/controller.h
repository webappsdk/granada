/**
  * Copyright (c) <2016> Web App SDK granada <afernandez@cookinapps.io>
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
  * Abstract class for controllers.
  * Controllers process the HTTP requests.
  */

#pragma once
#include "cpprest/http_listener.h"


using namespace web;
using namespace utility;
using namespace http;
using namespace web::http::experimental::listener;

namespace granada{
  namespace http{
    namespace controller{

      /**
       * Abstract class for controllers.
       * Controllers process the HTTP requests.
       */
      class Controller{

        public:

          /**
           * Controller
           */
          virtual ~Controller(){};


          /**
           * Opens and returns the listener.
           */
          virtual pplx::task<void> open() { return m_listener_->open(); };


          /**
           * Closes the listener.
           */
          virtual pplx::task<void> close() { return m_listener_->close(); };


        protected:

          /**
           * Listener
           */
          std::unique_ptr<web::http::experimental::listener::http_listener> m_listener_;

      };
    }
  }
}
