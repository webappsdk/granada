/**
  * Copyright (c) <2016> HTML Puzzle Team <htmlpuzzleteam@gmail.com>
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
  * Browse files and responds with the requested file.
  *
  */
#pragma once

#include "granada/cache/web_resource_cache.h"
#include "granada/http/controller/controller.h"
#include "granada/http/session/checkpoint.h"

namespace granada{
  namespace http{
    namespace controller{

      class BrowserController : public Controller
      {
      public:

        /**
         * Constructors
         */
        BrowserController(utility::string_t url);
        BrowserController(utility::string_t url,std::shared_ptr<granada::http::session::Checkpoint>& session_checkpoint);

      private:

        void handle_get(web::http::http_request request);

        /**
         * Cache Handler
         * Used to cache resources.
         */
        std::unique_ptr<granada::cache::WebResourceCache> cache_handler_;


        /**
         * Object for creating a session if it does not exist or if it is timed out.
         */
        std::shared_ptr<granada::http::session::Checkpoint> session_checkpoint_;
      };
    }
  }
}
