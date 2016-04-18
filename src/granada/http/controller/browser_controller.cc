/**
  * Copyright (c) <2016> Web App SDK granada <support@htmlpuzzle.com>
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
  */

#include "granada/http/controller/browser_controller.h"

namespace granada{
  namespace http{
    namespace controller{

      BrowserController::BrowserController(utility::string_t url){
        m_listener_ = std::unique_ptr<http_listener>(new http_listener(url));
        m_listener_->support(methods::GET, std::bind(&BrowserController::handle_get, this, std::placeholders::_1));
        cache_handler_ = std::unique_ptr<granada::cache::WebResourceCacheHandler>(new granada::cache::WebResourceCacheHandler());
        session_checkpoint_ = std::shared_ptr<granada::http::session::Checkpoint>(new granada::http::session::Checkpoint());
      }

      BrowserController::BrowserController(utility::string_t url,std::shared_ptr<granada::http::session::Checkpoint>& session_checkpoint){
        m_listener_ = std::unique_ptr<http_listener>(new http_listener(url));
        m_listener_->support(methods::GET, std::bind(&BrowserController::handle_get, this, std::placeholders::_1));
        cache_handler_ = std::unique_ptr<granada::cache::WebResourceCacheHandler>(new granada::cache::WebResourceCacheHandler());
        session_checkpoint_ = session_checkpoint;
      }

      //
      // A GET of the server browser get the files stored.
      //
      void BrowserController::handle_get(http_request request){

        std::string relative_uri_path = request.relative_uri().path();

        http_response response;

        session_checkpoint_->check(request,response);

        // retrieve a ressource with this a given path from cache.

        granada::cache::Resource resource = cache_handler_->GetFile(relative_uri_path);

        // check if this resource version has already been used by the client,
        // Tell the client if so using ETag.
        std::string if_none_match = request.headers()["If-None-Match"];

        response.headers().add(U("ETag"), resource.ETag);

        if (!if_none_match.empty() && if_none_match==resource.ETag){
          response.set_status_code(status_codes::NotModified);
        }else{
          try{
            // resource has not already been delivered.
            response.headers().add(U("Content-Encoding"),resource.content_encoding);
            response.headers().add(U("Server"), "granada");
            response.headers().add(U("Connection"), "keep-alive");
            response.headers().add(U("Last-Modified"), resource.last_modified);
            response.set_status_code(status_codes::OK);

            response.headers().add(U("Content-Type"), resource.content_type);
            response.set_body(resource.content);
          }catch(const std::exception& e){
            response.set_status_code(status_codes::NotFound);
          }
        }
        request.reply(response);
      }
    }
  }
}
