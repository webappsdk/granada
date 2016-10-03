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
  * SEO API. SEO builder for websites.
  *
  */

#include <stdio.h>
#include <string>
#include "granada/http/session/simple_session_checkpoint.h"
#include "granada/plugin/map_spidermonkey_plugin.h"
#include "granada/http/controller/browser_controller.h"
#include "granada/http/controller/plugin_controller.h"

////
// Vector containing all used controllers.
std::vector<std::unique_ptr<granada::http::controller::Controller>> g_controllers;

void on_initialize(const string_t& address)
{

  // session checkpoint used to create sessions.
  std::shared_ptr<granada::http::session::Checkpoint> session_checkpoint(new granada::http::session::SimpleSessionCheckpoint());

  ////
  // Browser Controller
  // Permits to browse server resources.
  // get property "browser_module" from the server configuration file
  // If this property equals "on" we will use browser controller.
  std::string browser_module = granada::util::application::GetProperty("browser_controller");
  if(!browser_module.empty() && browser_module=="on"){
    uri_builder uri(address);
    auto addr = uri.to_uri().to_string();
    std::unique_ptr<granada::http::controller::Controller> browser_controller(new granada::http::controller::BrowserController(addr,session_checkpoint));
    browser_controller->open().wait();
    g_controllers.push_back(std::move(browser_controller));
    ucout << "Browser Controller: Initialized... Listening for requests at: " << addr << std::endl;
  }

  // factory used to create Plug-in Handlers, Plug-ins, Plug-in Factories.
  std::shared_ptr<granada::plugin::PluginFactory> plugin_factory(new granada::plugin::MapSpidermonkeyPluginFactory());

  ////
  // Plugin Controller
  //
  uri_builder plugin_uri(address);
  plugin_uri.append_path(U("plugin"));
  auto addr = plugin_uri.to_uri().to_string();
  std::unique_ptr<granada::http::controller::PluginController> plugin_controller(new granada::http::controller::PluginController(addr,session_checkpoint,plugin_factory));
  plugin_controller->open().wait();
  g_controllers.push_back(std::move(plugin_controller));
  ucout << "Plugin Controller: Initialized... Listening for requests at: " << addr << std::endl;

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

  std::cout << "------------------- PLUGIN TEST -------------------" << std::endl;

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
