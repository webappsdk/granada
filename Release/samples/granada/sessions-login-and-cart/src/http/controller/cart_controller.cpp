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
  * Controlle for managing cart.
  *
  */

#include "cart_controller.h"

namespace granada{
  namespace http{
    namespace controller{
      CartController::CartController(utility::string_t url)
      {
        m_listener_ = std::unique_ptr<http_listener>(new http_listener(url));
        m_listener_->support(methods::GET, std::bind(&CartController::handle_get, this, std::placeholders::_1));
        m_listener_->support(methods::PUT, std::bind(&CartController::handle_put, this, std::placeholders::_1));
        m_listener_->support(methods::POST, std::bind(&CartController::handle_post, this, std::placeholders::_1));
      }

      void CartController::handle_get(web::http::http_request request)
      {

        web::http::http_response response;

        granada::http::session::MapSession simple_session(request,response);
        business::Cart cart(&simple_session);

        auto paths = uri::split_path(uri::decode(request.relative_uri().path()));
        if (!paths.empty()){
          std::string name = paths[0];

          if(name == "count"){

            // returns the number of products added to the cart.
            int count = cart.Count();
            response.set_body("{\"count\":\"" + std::to_string(count) + "\"}");

          }else if (name == "list"){

            // returns a list with the products added to the cart but only
            // if the user is logged in.
            if (simple_session.roles()->Is("USER")){
              response.set_body("{\"data\":" + cart.List() + "}");
            }else{
              response.set_body("{\"data\":[]}");
            }
          }
        }

        response.set_status_code(status_codes::OK);
        response.headers().add(U("Content-Type"), U("text/json; charset=utf-8"));

        request.reply(response);
      }

      void CartController::handle_put(web::http::http_request request)
      {
        web::http::http_response response;
        granada::http::session::MapSession simple_session(request,response);
        business::Cart cart(&simple_session);

        std::string product_id = "";
        int quantity = 0;
        std::string body = request.extract_utf8string(true).get();
        std::unordered_map<std::string,std::string> parsed_query = granada::http::parser::ParseQueryString(body);

        auto it = parsed_query.find("id");
        if (it != parsed_query.end()){
          product_id.assign(it->second);
        }

        auto it2 = parsed_query.find("quantity");
        if (it2 != parsed_query.end()){
          quantity = std::atoi(it2->second.c_str());
        }

        int count = 0;
        if (product_id.empty()){

          // invalid product id, do not add the product,
          // just return the number of products already added.
          count = cart.Count();

        }else{

          // adds a product to the cart.
          count = cart.Add(product_id,quantity);

        }

        response.set_status_code(status_codes::OK);
        response.headers().add(U("Content-Type"), U("text/json; charset=utf-8"));
        response.set_body("{\"count\":\"" + std::to_string(count) + "\"}");
        request.reply(response);
      }

      void CartController::handle_post(web::http::http_request request)
      {
        web::http::http_response response;
        granada::http::session::MapSession simple_session(request,response);
        business::Cart cart(&simple_session);

        std::string product_id = "";
        int quantity = 0;
        std::string body = request.extract_utf8string(true).get();
        std::unordered_map<std::string,std::string> parsed_query = granada::http::parser::ParseQueryString(body);

        auto it = parsed_query.find("id");
        if (it != parsed_query.end()){
          product_id.assign(it->second);
        }

        auto it2 = parsed_query.find("quantity");
        if (it2 != parsed_query.end()){
          quantity = std::atoi(it2->second.c_str());
        }

        int count = 0;
        if (product_id.empty()){

          // invalid product id, do not modify the product,
          // just return the number of products already added.
          count = cart.Count();

        }else{

          // modify the product already added in the cart.
          count = cart.Edit(product_id,quantity);

        }

        response.set_status_code(status_codes::OK);
        response.headers().add(U("Content-Type"), U("text/json; charset=utf-8"));
        response.set_body("{\"count\":\"" + std::to_string(count) + "\"}");
        request.reply(response);
      }
    }
  }
}
