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
  * Oauth2 Controller
  *
  */

#pragma once
#include <unordered_map>
#include <fstream>
#include "cpprest/json.h"
#include "granada/util/string.h"
#include "granada/http/parser.h"
#include "granada/http/http_msg.h"
#include "granada/http/oauth2/oauth2.h"
#include "granada/http/controller/controller.h"
#include "granada/http/session/checkpoint.h"

namespace granada{
  namespace http{
    namespace controller{
      class OAuth2Controller : public Controller {
        public:

          /**
           * Constructor
           * @param url                   Adress of the controller.
           * @param session_checkpoint    Allows to have a unique point for checking and setting sessions.
           *                              Can be used to create a new session if it does not exist.
           * @param oauth2_factory        Used to instanciate OAuth 2.0 clients, users and codes.
           */
          OAuth2Controller(
            utility::string_t url,
            std::shared_ptr<granada::http::session::Checkpoint>& session_checkpoint,
            std::shared_ptr<granada::http::oauth2::OAuth2Factory>& oauth2_factory);


        private:

          /**
           * URL of the controller.
           */
          utility::string_t url_;


          /**
           * Allows to have a unique point for checking and setting sessions.
           * Can be used to create a new session if it does not exist.
           */
          std::shared_ptr<granada::http::session::Checkpoint> session_checkpoint_;


          /**
           * OAuth 2.0 Factory
           * Used to instanciate OAuth 2.0 clients, users and codes.
           */
          std::shared_ptr<granada::http::oauth2::OAuth2Factory> oauth2_factory_;


          /**
           * Sub URI address for the authorization page.
           */
          std::string oauth2_authorize_uri_;


          /**
           * Sub URI address for loggin out users.
           */
          std::string oauth2_logout_uri_;


          /**
           * Sub URI address to obtain information about the authorized
           * clients of a user.
           */
          std::string oauth2_info_uri_;


          /**
           * HTML of the authorizing login page to show when the user
           * is not already logged.
           */
          std::shared_ptr<std::string> oauth2_authorizing_login_template_;


          /**
           * HTML of the authorizing login page to show when the user
           * is already logged.
           */
          std::shared_ptr<std::string> oauth2_authorizing_message_template_;


          /**
           * HTML of the page to show when a user loggout from the
           * authorization server.
           */
          std::shared_ptr<std::string> oauth2_logout_template_;


          /**
           * HTML of the page to show when a user tries to access a wrong
           * URL.
           */
          std::shared_ptr<std::string> oauth2_authorizing_error_template_;


          /**
          * Load the templates and URIs from the server configuration file, if properties are not in this
          * file then take default values from granada/defaults.dat and granada/http/oauth2/oauth2.templates.
          * Properties:
          *
          * - oauth2_authorize_uri_      Sub address of the oauth 2 controller,
          *                              Example:
          *                            			address: http://localhost:80/oauth2
          *                            		 	auth_uri: auth
          *                            		  |=> total address: http://localhost:80/oauth2/auth
          *                                Standards usually are: "auth" or "authorize"
          *
          *
          * - oauth2_logout_uri_        Sub address to logout users from the authorization server.
          *
          * - oauth2_info_uri_          Sub address to obtain information about a user authorized clients.
          *
          * - login_template_path       The path of the file containing the HTML template to authorize clients
          *                              by users that are not already logged.
          *                              The content of the template has to be something like this
          *                              (note the {{tag}} wil be replaced by OAuth 2.0 parameter values):
          *        										       <form id="authorization-form" method="POST" action="/oauth2/" enctype="application/x-www-form-urlencoded">
          * 		               										<input type="text" name="username" placeholder="username"></input><br />
          *   	                     							<input type="password" name="password" placeholder="password"></input><br />
          *                             		 			<input type="hidden" name="response_type" value="{{response_type}}"></input>
          *                               	   		<input type="hidden" name="scope" value="{{scope}}"></input>
          *                                     	<input type="hidden" name="client_id" value="{{client_id}}"></input>
          *                                      <input type="hidden" name="redirect_uri" value="{{redirect_uri}}"></input>
          *                                      <input type="hidden" name="state" value="{{state}}"></input>
          *                                      <button>Authorize</button><button type="button">Cancel</button>
          *                                   </form>
          *
          * - message_template_path     The path of the file containing the HTML template to authorize clients
          *                              by users that are already logged.
          *                              The content of the template has to be something like this
          *                              (note the {{tag}} wil be replaced by OAuth 2.0 parameter values):
          *        										       <form id="authorization-form" method="POST" action="/oauth2/" enctype="application/x-www-form-urlencoded">
          *                                			<input type="hidden" name="response_type" value="{{response_type}}"></input>
          *                                   		<input type="hidden" name="scope" value="{{scope}}"></input>
          *                                     	<input type="hidden" name="client_id" value="{{client_id}}"></input>
          *                                      <input type="hidden" name="redirect_uri" value="{{redirect_uri}}"></input>
          *                                      <input type="hidden" name="state" value="{{state}}"></input>
          *                                      <input type="hidden" name="authorize" value="authorize"></input>
          *                                      <button>Allow</button> <button>Do not allow</button>
          *                                   </form>
          *
          * - error_template_path       The path of the file containing the HTML template to show when user try to access oauth2 with a wrong URI.
          *
          * -
          *
          * - use_refresh_token         If true a refresh_token is sent back when a token is asked with a code grant type.
          */
          void LoadProperties();


          /**
           * Loads a particular HTML template.
           * @param property_name    Name of the property in the server configuration file containing the path of the template file.
           * @param default_template HTML to use in case no path is given in the server configuration file.
           * @param html_template    Pointer of the string to fill with the HTML of the template.
           */
          void LoadHTMLTemplate(const std::string& property_name, const std::string& default_template, std::shared_ptr<std::string>& html_template);


          /**
           * Handles HTTP GET requests.
           * @param request HTTP request.
           */
          void handle_get(http_request request);


          /**
           * Handles HTTP POST requests.
           * @param request HTTP request.
           */
          void handle_post(http_request request);


          /**
           * Handles HTTP DELETE requests.
           * @param request HTTP request.
           */
          void handle_delete(http_request request);

      };
    }
  }
}
