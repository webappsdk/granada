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
  * Server for browsing content in the server side, handling plugins.
  */

#include <stdio.h>
#include <string>
#include "granada/http/session/map_session.h"
#include "granada/http/controller/browser_controller.h"
#include "src/http/controller/cart_controller.h"
#include "src/http/controller/auth_controller.h"

////
// Vector containing all used controllers.
std::vector<std::unique_ptr<granada::http::controller::Controller>> g_controllers;

void on_initialize(const string_t& address)
{

  ////
  // Browser Controller
  // Permits to browse server resources.

  // get property "browser_module" from the server configuration file
  // If this property equals "on" we will use browser controller.
  std::string browser_module = granada::util::application::GetProperty("browser_controller");
  if(!browser_module.empty() && browser_module=="on"){

    std::shared_ptr<granada::http::session::SessionFactory> map_simple_session_factory(new granada::http::session::MapSessionFactory());

    uri_builder uri(address);
    auto addr = uri.to_uri().to_string();
    std::unique_ptr<granada::http::controller::Controller> browser_controller(new granada::http::controller::BrowserController(addr,map_simple_session_factory));
    browser_controller->open().wait();
    g_controllers.push_back(std::move(browser_controller));
    ucout << "Browser Controller: Initialized... Listening for requests at: " << addr << std::endl;
  }

  uri_builder uri(address);
  uri.append_path(U("cart"));
  auto addr = uri.to_uri().to_string();
  std::unique_ptr<granada::http::controller::CartController> cart_controller(new granada::http::controller::CartController(addr));
  cart_controller->open().wait();
  g_controllers.push_back(std::move(cart_controller));
  ucout << "Cart Controller: Initialized... Listening for requests at: " << addr << std::endl;


  uri_builder auth_uri(address);
  auth_uri.append_path(U("auth"));
  addr = auth_uri.to_uri().to_string();
  std::unique_ptr<granada::http::controller::AuthController> auth_controller(new granada::http::controller::AuthController(addr));
  auth_controller->open().wait();
  g_controllers.push_back(std::move(auth_controller));
  ucout << "Auth Controller: Initialized... Listening for requests at: " << addr << std::endl;


  return;
}

void on_shutdown()
{
  for(auto const& controller : g_controllers){
    controller->close().wait();
  }
  return;
}


#ifdef _WIN32
int wmain(int argc, wchar_t *argv[])
#else
int main(int argc, char *argv[])
#endif
{

  std::cout << "------------------- granada -------------------" << std::endl;

  std::string port_str = granada::util::application::GetProperty("port");
  if (port_str.empty()){
    port_str = "80";
  }
  utility::string_t port = U(port_str);
  if(argc == 2)
  {
    port = argv[1];
  }

  std::string address_str = granada::util::application::GetProperty("address");
  if (address_str.empty()){
    address_str = "http://localhost:";
  }else{
    address_str += ":";
  }

  utility::string_t address = U(address_str);
  address.append(port);

  on_initialize(address);

  std::cout << "------------------------------------------------\nPress ENTER to terminate server." << std::endl;

  std::string line;
  std::getline(std::cin, line);

  on_shutdown();

  return 0;
}
