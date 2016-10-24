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
  * Example of oauth2 server.
  * Resource Owner Password Credentials Grant model.
  *
  */


#include <memory>
#include <stdio.h>
#include <string>
#include <vector>
#include "granada/cache/redis_cache_driver.h"
#include "granada/http/session/redis_session.h"
#include "granada/http/oauth2/redis_oauth2.h"
#include "cpprest/details/basic_types.h"
#include "granada/http/controller/browser_controller.h"
#include "granada/http/controller/oauth2_controller.h"
#include "src/http/controller/user_controller.h"
#include "src/http/controller/client_controller.h"
#include "src/http/controller/message_controller.h"
#include "src/http/controller/application_controller.h"

////
// Vector containing all used controllers.
std::vector<std::unique_ptr<granada::http::controller::Controller>> g_controllers;

void on_initialize(const string_t& address)
{

  std::shared_ptr<granada::http::session::SessionFactory> session_factory(new granada::http::session::RedisSessionFactory());
  
  std::shared_ptr<granada::http::oauth2::OAuth2Factory> oauth2_factory(new granada::http::oauth2::RedisOAuth2Factory());
  
  ////
  // Browser Controller
  // Permits to browse server resources.

  // get property "browser_module" from the server configuration file
  // If this property equals "on" we will use browser controller.
  std::string browser_module = granada::util::application::GetProperty("browser_controller");
  if(!browser_module.empty() && browser_module=="on"){
    uri_builder uri(address);
    auto addr = uri.to_uri().to_string();
    std::unique_ptr<granada::http::controller::Controller> browser_controller(new granada::http::controller::BrowserController(addr,session_factory));
    browser_controller->open().wait();
    g_controllers.push_back(std::move(browser_controller));
    ucout << "Browser Controller: Initialized... Listening for requests at: " << addr << std::endl;
  }


  ////
  // User Controller
  // Used for registering users.
  uri_builder user_uri(address);
  user_uri.append_path(U("user"));
  auto addr = user_uri.to_uri().to_string();
  std::unique_ptr<granada::http::controller::UserController> user_controller(new granada::http::controller::UserController(addr,oauth2_factory));
  user_controller->open().wait();
  g_controllers.push_back(std::move(user_controller));
  ucout << "User Controller: Initialized... Listening for requests at: " << addr << std::endl;


  ////
  // Client Controller
  // Used for registering clients.
  uri_builder client_uri(address);
  client_uri.append_path(U("client"));
  addr = client_uri.to_uri().to_string();
  std::unique_ptr<granada::http::controller::ClientController> client_controller(new granada::http::controller::ClientController(addr,oauth2_factory));
  client_controller->open().wait();
  g_controllers.push_back(std::move(client_controller));
  ucout << "Client Controller: Initialized... Listening for requests at: " << addr << std::endl;


  ////
  // Auth Controller
  // Resource Owner Password Credentials Grant
  //

  uri_builder auth_uri(address);
  auth_uri.append_path(U("oauth2"));
  addr = auth_uri.to_uri().to_string();
  std::unique_ptr<granada::http::controller::OAuth2Controller> auth_controller(new granada::http::controller::OAuth2Controller(addr,session_factory,oauth2_factory));
  auth_controller->open().wait();
  g_controllers.push_back(std::move(auth_controller));
  ucout << "Auth Controller: Initialized... Listening for requests at: " << addr << std::endl;

 
  ////
  // Message Controller
  // Used for listing, inserting, editing, deleting users' messages.
  std::shared_ptr<granada::cache::CacheHandler> cache_handler = std::make_shared<granada::cache::RedisCacheDriver>();
  uri_builder message_uri(address);
  message_uri.append_path(U("message"));
  addr = message_uri.to_uri().to_string();
  std::unique_ptr<granada::http::controller::MessageController> message_controller(new granada::http::controller::MessageController(addr,session_factory,cache_handler));
  message_controller->open().wait();
  g_controllers.push_back(std::move(message_controller));
  ucout << "Message Controller: Initialized... Listening for requests at: " << addr << std::endl;
  

  ////
  // Application Controller
  // Application for message reading and edition.
  uri_builder application_uri(address);
  application_uri.append_path(U("application"));
  addr = application_uri.to_uri().to_string();
  std::unique_ptr<granada::http::controller::ApplicationController> application_controller(new granada::http::controller::ApplicationController(addr,session_factory));
  application_controller->open().wait();
  g_controllers.push_back(std::move(application_controller));
  ucout << "Application Controller: Initialized... Listening for requests at: " << addr << std::endl;
  
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

	std::cout << "------------------- REDIS OAUTH2 SERVER EXAMPLE -------------------" << std::endl;

	std::string port_str = granada::util::application::GetProperty("port");
	if (port_str.empty()){
		port_str = "80";
	}
	utility::string_t port = utility::conversions::to_string_t(port_str);

	std::string address_str = granada::util::application::GetProperty("address");
	if (address_str.empty()){
		address_str = "http://localhost:";
	}
	else{
		address_str += ":";
	}

	utility::string_t address = utility::conversions::to_string_t(address_str);
	address.append(port);

	on_initialize(address);

	std::cout << "------------------------------------------------\nPress ENTER to terminate server." << std::endl;

	std::string line;
	std::getline(std::cin, line);

	on_shutdown();

	std::cout << "bye,bye.\n\n";

  return 0;
}
