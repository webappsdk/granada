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
  * Browses files in the server using web resource cache
  * and responds with the requested file.
  *
  */
#pragma once

#include "granada/cache/web_resource_cache.h"
#include "granada/http/controller/controller.h"
#include "granada/http/session/session.h"

namespace granada{
  namespace http{
    namespace controller{

      /**
       * Browses files in the server using web resource cache
       * and responds with the requested file.
       */
      class BrowserController : public Controller
      {

      public:


        /**
         * Constructor
         * @param   url  URI the controller listens to.
         */
        BrowserController(utility::string_t url);


        /**
         * Constructor
         * @param   url                 URI the controller listens to.
         * @param   session_factory  Session factory. Allows to have a unique point for
         *                              checking and setting sessions. Used to create a new
         *                              session if it does not exist or if it is timed out.
         */
        BrowserController(utility::string_t url,std::shared_ptr<granada::http::session::SessionFactory>& session_factory);


        /**
         * Destructor
         */
        virtual ~BrowserController(){};


      private:


        // override
        void handle_get(web::http::http_request request);


        /**
         * Web resource cache
         * Used to cache resources.
         */
        std::unique_ptr<granada::cache::WebResourceCache> cache_handler_;


        /**
         * Session factory. Allows to have a unique point for
         * checking and setting sessions. Used to create a new
         * session if it does not exist or if it is timed out.
         */
        std::shared_ptr<granada::http::session::SessionFactory> session_factory_;
        
      };
    }
  }
}
