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
  * Classes of entities useful for OAuth 2.0 authorization,
  * for the server or the client.
  * Based on rfc6749 document: The OAuth 2.0 Authorization Framework
  * https://tools.ietf.org/html/rfc6749
  *
  */

#pragma once

#include <string>
#include <unordered_map>
#include "cpprest/json.h"
#include "cpprest/http_msg.h"
#include "cpprest/oauth2.h"
#include "granada/defaults.h"
#include "granada/util/vector.h"
#include "granada/util/application.h"
#include "granada/util/time.h"
#include "granada/http/parser.h"
#include "granada/http/session/checkpoint.h"
#include "granada/http/session/session.h"
#include "granada/cache/cache_handler.h"
#include "granada/crypto/cryptograph.h"
#include "granada/crypto/nonce_generator.h"

/**
 * OAuth 2.0 authorization errors.
 */
typedef utility::string_t oauth2_error;
class oauth2_errors
{
public:
#define _OAUTH2_ERRORS
#define DAT(a_, b_) _ASYNCRTIMP static const oauth2_error a_;
#include "granada/http/http_constants.dat"
#undef _OAUTH2_ERRORS
#undef DAT
};


/**
 * OAuth 2.0 authorization errors descriptions.
 */
typedef utility::string_t oauth2_error_description;
class oauth2_errors_description
{
public:
#define _OAUTH2_ERRORS_DESCRIPTION
#define DAT(a_, b_) _ASYNCRTIMP static const oauth2_error_description a_;
#include "granada/http/http_constants.dat"
#undef _OAUTH2_ERRORS_DESCRIPTION
#undef DAT
};


/**
 * OAuth 2.0 client types
 */
typedef utility::string_t oauth2_client_type;
class oauth2_client_types
{
public:
#define _OAUTH2_CLIENT_TYPES
#define DAT(a_, b_) _ASYNCRTIMP static const oauth2_client_type a_;
#include "granada/http/http_constants.dat"
#undef _OAUTH2_CLIENT_TYPES
#undef DAT
};


/**
 * Extension of the OAuth 2.0 strings. Includes the
 * useful server authorization constants.
 */
typedef utility::string_t oauth2_string_2;
class oauth2_strings_2
{
public:
#define _OAUTH2_STRINGS_2
#define DAT(a_, b_) _ASYNCRTIMP static const oauth2_string_2 a_;
#include "granada/http/http_constants.dat"
#undef _OAUTH2_STRINGS_2
#undef DAT
};

/**
 * OAuth 2.0 HTML templates, this templates are used in case
 * the user does not provide custom ones.
 */
typedef utility::string_t oauth2_template;
class oauth2_templates
{
public:
#define _TEMPLATES
#define TEMPLATES(a_, b_) _ASYNCRTIMP static const oauth2_template a_;
#include "granada/http/oauth2/oauth2.templates"
#undef _TEMPLATES
#undef TEMPLATES
};

using namespace web::http::oauth2::details;

namespace granada{

  namespace http{

    namespace oauth2{

      class OAuth2Factory;

      /**
       * OAuth2 Parameters and parsing and encoding functions.
       * Parameters for use with the authorization endpoint,
       * the token endpoint and the information and deletion endpoint.
       */
      class OAuth2Parameters{
        public:

          /**
           * Constructor
           */
          OAuth2Parameters(){};


          /**
           * Constructor
           * Parse a query string from an HTTP request and fill the oauth2 parameters.
           * @param query_string Query string from HTTP request.
           */
          OAuth2Parameters(const std::string& query_string){
            try{
              std::unordered_map<std::string, std::string>  parsed_data =	granada::http::parser::ParseQueryString(query_string);
              username.assign(parsed_data[oauth2_strings_2::username]);
              password.assign(parsed_data[oauth2_strings_2::password]);
              code.assign(parsed_data[oauth2_strings::code]);
              authorize.assign(parsed_data[oauth2_strings_2::authorize]);
              access_token.assign(parsed_data[oauth2_strings::access_token]);
              expires_in.assign(parsed_data[oauth2_strings::expires_in]);
              refresh_token.assign(parsed_data[oauth2_strings::refresh_token]);
              token_type.assign(parsed_data[oauth2_strings::token_type]);
              grant_type.assign(parsed_data[oauth2_strings::grant_type]);
              response_type.assign(parsed_data[oauth2_strings::response_type]);
              client_id.assign(parsed_data[oauth2_strings::client_id]);
              client_secret.assign(parsed_data[oauth2_strings::client_secret]);
              redirect_uri.assign(parsed_data[oauth2_strings::redirect_uri]);
              scope.assign(parsed_data[oauth2_strings::scope]);
              state.assign(parsed_data[oauth2_strings::state]);
              error.assign(parsed_data[oauth2_errors::error]);
              error_description.assign(parsed_data[oauth2_errors::error_description]);
            }catch(const std::exception& e){}
          };


          /**
           * Convert OAuth 2.0 parameters into a unordered_map.
           *
           * @return    OAuth 2.0 parameters in form of map.
           */
          std::unordered_map<std::string,std::string> to_unordered_map(){
            std::unordered_map<std::string,std::string> map;
            if (!code.empty()){ map.insert(std::make_pair(oauth2_strings::code,code)); }
            if (!access_token.empty()){ map.insert(std::make_pair(oauth2_strings::access_token,access_token)); }
            if (!expires_in.empty()){ map.insert(std::make_pair(oauth2_strings::expires_in,expires_in)); }
            if (!refresh_token.empty()){ map.insert(std::make_pair(oauth2_strings::refresh_token,refresh_token)); }
            if (!token_type.empty()){ map.insert(std::make_pair(oauth2_strings::token_type,token_type)); }
            if (!grant_type.empty()){ map.insert(std::make_pair(oauth2_strings::grant_type,grant_type)); }
            if (!response_type.empty()){ map.insert(std::make_pair(oauth2_strings::response_type,response_type)); }
            if (!client_id.empty()){ map.insert(std::make_pair(oauth2_strings::client_id,client_id)); }
            if (!redirect_uri.empty()){ map.insert(std::make_pair(oauth2_strings::redirect_uri,redirect_uri)); }
            if (!scope.empty()){ map.insert(std::make_pair(oauth2_strings::scope,scope)); }
            if (!state.empty()){ map.insert(std::make_pair(oauth2_strings::state,state)); }
            if (!error.empty()){ map.insert(std::make_pair(oauth2_errors::error,error)); }
            if (!error_description.empty()){ map.insert(std::make_pair(oauth2_errors::error_description,error_description)); }
            return map;
          }


          /**
           * Convert OAuth 2.0 parameters into a query string.
           * Example:
           * 	?access_token=k5g25AGZcIfjduQ9vkLTzUXGbnBjbQ4RaJCfyOz02OiGlqhiIjYmR6tGbwJlQ0Gf&token_type=bearer&scope=MSG_INSERT%20MSG_SELECT%20&state=buPgXS4W1vVi7WUX
           *
           * @return OAuth 2.0 parameters in form of query string.
           */
          std::string to_query_string(){
            std::string query_string = "";
            if (!code.empty()){ query_string+="&"+oauth2_strings::code+"="+code; }
            if (!access_token.empty()){ query_string+="&"+oauth2_strings::access_token+"="+access_token; }
            if (!expires_in.empty()){ query_string+="&"+oauth2_strings::expires_in+"="+expires_in; }
            if (!refresh_token.empty()){ query_string+="&"+oauth2_strings::refresh_token+"="+refresh_token; }
            if (!token_type.empty()){ query_string+="&"+oauth2_strings::token_type+"="+token_type; }
            if (!grant_type.empty()){ query_string+="&"+oauth2_strings::grant_type+"="+grant_type; }
            if (!response_type.empty()){ query_string+="&"+oauth2_strings::response_type+"="+response_type; }
            if (!client_id.empty()){ query_string+="&"+oauth2_strings::client_id+"="+client_id; }
            if (!redirect_uri.empty()){ query_string+="&"+oauth2_strings::redirect_uri+"="+redirect_uri; }
            if (!scope.empty()){ query_string+="&"+oauth2_strings::scope+"="+scope; }
            if (!state.empty()){ query_string+="&"+oauth2_strings::state+"="+state; }
            if (!error.empty()){ query_string+="&"+oauth2_errors::error+"="+error; }
            if (!error_description.empty()){ query_string+="&"+oauth2_errors::error_description+"="+error_description; }
            if (!query_string.empty()){
              query_string[0] = '?';
            }
            return query_string;
          }


          /**
           * Convert OAuth 2.0 parameters into a json.
           * 	Example:
           * 		{
           * 			"access_token":"2YotnFZFEjr1zCsicMWpAA",
           * 			"token_type":"example",
           * 			"expires_in":3600,
           * 			"refresh_token":"tGzv3JOkF0XG5Qx2TlKWIA",
           * 			"example_parameter":"example_value"
           * 		}
           *
           * @return OAuth 2.0 parameters in form of json.
           */
          web::json::value to_json(){
            std::string json_str = "";
            if (!code.empty()){ json_str+=",\""+oauth2_strings::code+"\":\""+code+"\""; }
            if (!access_token.empty()){ json_str+=",\""+oauth2_strings::access_token+"\":\""+access_token+"\""; }
            if (!expires_in.empty()){ json_str+=",\""+oauth2_strings::expires_in+"\":\""+expires_in+"\""; }
            if (!refresh_token.empty()){ json_str+=",\""+oauth2_strings::refresh_token+"\":\""+refresh_token+"\""; }
            if (!token_type.empty()){ json_str+=",\""+oauth2_strings::token_type+"\":\""+token_type+"\""; }
            if (!grant_type.empty()){ json_str+=",\""+oauth2_strings::grant_type+"\":\""+grant_type+"\""; }
            if (!response_type.empty()){ json_str+=",\""+oauth2_strings::response_type+"\":\""+response_type+"\""; }
            if (!client_id.empty()){ json_str+=",\""+oauth2_strings::client_id+"\":\""+client_id+"\""; }
            if (!redirect_uri.empty()){ json_str+=",\""+oauth2_strings::redirect_uri+"\":\""+redirect_uri+"\""; }
            if (!scope.empty()){ json_str+=",\""+oauth2_strings::scope+"\":\""+scope+"\""; }
            if (!state.empty()){ json_str+=",\""+oauth2_strings::state+"\":\""+state+"\""; }
            if (!error.empty()){ json_str+=",\""+oauth2_errors::error+"\":\""+error+"\""; }
            if (!error_description.empty()){ json_str+=",\""+oauth2_errors::error_description+"\":\""+error_description+"\""; }
            if (json_str.empty()){
              json_str = "{}";
            }else{
              json_str[0] = '{';
              json_str += "}";
            }
            return web::json::value::parse(json_str);
          }


          std::string username;
          std::string password;
          std::string code;
          std::string authorize;
          std::string access_token;
          std::string expires_in;
          std::string refresh_token;
          std::string token_type;
          std::string grant_type;
          std::string response_type;
          std::string client_id;
          std::string client_secret;
          std::string redirect_uri;
          std::string scope;
          std::string state;
          std::string error;
          std::string error_description;
      };


      /**
       * Abstract class of the OAuth 2.0 entities.
       */
      class OAuth2Entity{
        public:

          /**
           * Returns the used cache: where entities data is stored.
           * @return Cache where data is stored.
           */
          virtual std::shared_ptr<granada::cache::CacheHandler> cache(){
            return std::shared_ptr<granada::cache::CacheHandler>(nullptr);
          };


          /**
           * Returns the cryptograph used to encrypt and decrypt data.
           * return Cryptograph.
           */
          virtual std::shared_ptr<granada::crypto::Cryptograph> cryptograph(){
            return std::shared_ptr<granada::crypto::Cryptograph>(nullptr);
          };


          /**
           * Returns the nonce generator used to generate alphanumeric keys.
           * return Nonce generator.
           */
          virtual std::shared_ptr<granada::crypto::NonceGenerator> nonce_generator(){
            return std::shared_ptr<granada::crypto::NonceGenerator>(nullptr);
          };


          /**
           * Returns true if OAuth 2.0 entity exists wherever it is stored.
           * @return True if OAuth 2.0 entity exists false if it does not.
           */
          virtual bool Exists(){
            return cache()->Exists(hash());
          };


          /**
           * Loads the data of the OAuth 2.0 entity with the existant entity identifier.
           */
          virtual void Load(){};


          /**
           * Loads the data of the OAuth 2.0 entity into the variables of the class
           * with given identifier.
           * @param identifier Identifier of the OAuth 2.0 client, user or code.
           */
          virtual void Load(const std::string& identifier){};


        protected:

          /**
           * Namespace of the key of the entity data in the cache.
           * Example:
           * 	If we have the key : oauth2.client:value:L05l6pFaPFgZbtP9
           * 	=> namespace is : oauth2.client:value:
           */
          std::string cache_namespace_;


          /**
           * Mutex for multithread safety.
           */
          std::mutex mtx;


          /**
           * Returns the key of the data : that is the namespace and the identifier
           * @return Key of the entity values.
           */
          virtual std::string hash(){
            return std::string();
          };

          /**
           * Loads properties given in the configuration file, if properties
           * are not found, then default values included in granada/defaults.dat
           * file are used.
           */
          virtual void LoadProperties(){};
      };


      /**
       * OAuth 2.0 client. The client represents the application that will access the user's resources.
       */
      class OAuth2Client : public OAuth2Entity{
        public:


          /**
           * Constructor
           * Loads the properties.
           */
          OAuth2Client();


          /**
           * Constructor
           * Loads the properties and the values of the client with given id.
           * @param id Identifier of the client.
           */
          OAuth2Client(const std::string& id);


          /**
           * @override
           * Loads the values of the client retrieving them from the cache
           * with the client id.
           */
          virtual void Load();


          /**
           * @override
           * Loads the values of the client retrieving them from the cache
           * with the given client id.
           * @param identifier Client id.
           */
          virtual void Load(const std::string& identifier);


          /**
           * Creates a new client and store it using the cache.
           * Client values will be stored with a key like: oauth2.client:value:myfNv849Z1GNuPAN.
           * @param type             Type of client: public | confidential
           *                         public:        Clients incapable of maintaining the confidentiality of their
           *                                  			credentials (e.g., clients executing on the device used by the
           *                                  			resource owner, such as an installed native application or a web
           *                                  			browser-based application), and incapable of secure client
           *                                  			authentication via any other means.
           *                         confidential:  Clients capable of maintaining the confidentiality of their
           *                                        credentials (e.g., client implemented on a secure server with
           *                                        restricted access to the client credentials), or capable of secure
           *                                        client authentication using other means.
           * @param redirect_uris    Uris to redirect the user after he has authorize or not the client.
           *                         The redirection endpoint URI MUST be an absolute URI
           * @param application_name Name of the application.
           * @param roles            Maximum roles the client can ask to have, roles manage the permissions
           *                         over the user's resources.
           *                         Example:
           *                         		MSG_INSERT => this role will give the client the permission to create user messages.
           * @param secret          Password of the client, the client can use it to ask for a OAuth 2.0 code.
           */
          virtual void Create(const std::string& type, const std::vector<std::string>& redirect_uris, const std::string& application_name, const std::vector<std::string>& roles, std::string& secret);


          /**
           * Checl if client credentials: id and secret are correct decrypting
           * the key stored in the client key property. Use the cryptograph to
           * decrypt the key with the given client secret.
           * @param  secret Client password.
           * @return        True if credentials are correct, false if they are not.
           */
          virtual bool CorrectCredentials(std::string& secret);


          /**
           * Delets an OAuth 2.0 client.
           * @param  secret   Client password.
           * @return          True if client is deleted successfuly, false if it not.
           */
          virtual bool Delete(const std::string& secret);


          /**
           * Returns true if the client has the given redirect URI in its collection.
           * A client can have multiple redirect URI, in OAuth 2.0 authorization
           * if no redirect_uri is provided, the first one in the client collection is taken.
           * @param  redirect_uri Redirect URI, check if the client has it.
           * @return              True if client has the given redirection URI in its collection,
           *                      False if it does not.
           */
          virtual const bool HasRedirectURI(const std::string& redirect_uri){
            return (std::find(redirect_uris_.begin(), redirect_uris_.end(), redirect_uri) != redirect_uris_.end());
          };


          /**
           * Returns true if client has the given role in its collection.
           * False if it does not.
           * @param  role Role to check.
           * @return      True if client has the given role in its collection, false if it does not.
           */
          virtual const bool HasRole(const std::string& role){
            return std::find(roles_.begin(), roles_.end(), role) != roles_.end();
          };


          virtual const std::string GetId(){
            return id_;
          };


          virtual void SetId(const std::string& id){
            id_.assign(id);
          };


          virtual const std::string GetType(){
            return type_;
          };

          virtual const std::string GetApplicationName(){
            return application_name_;
          };

          virtual const std::vector<std::string> GetRedirectURIs(){
            return redirect_uris_;
          };


          virtual const std::vector<std::string> GetRoles(){
            return roles_;
          };

          virtual const std::time_t GetCreationTime(){
            return creation_time_;
          };


        protected:


          /**
           * Client id. Unique alphanumeric hash.
           */
          std::string id_;


          /**
           * Crypted key, used with the client secret to verify the client credentials
           * when needed.
           */
          std::string key_;


          /**
           * Type of client: public | confidential
           * public:        Clients incapable of maintaining the confidentiality of their
           *          			credentials (e.g., clients executing on the device used by the
           *          			resource owner, such as an installed native application or a web
           *          			browser-based application), and incapable of secure client
           *          			authentication via any other means.
           * confidential:  Clients capable of maintaining the confidentiality of their
           *                credentials (e.g., client implemented on a secure server with
           *                restricted access to the client credentials), or capable of secure
           *                client authentication using other means.
           */
          std::string type_;


          /**
           * Name of the client application, used like a client description field,
           * not used for authorization purposes.
           */
          std::string application_name_;


          /**
           * URI to which the user will be redirected once he authorize or not
           * the client.
           */
          std::vector<std::string> redirect_uris_;


          /**
           * Maximum roles a user can be authorize to have when authorized,
           * roles manage the permissions an entity has over user's resources.
           */
          std::vector<std::string> roles_;


          /**
           * Date when client is created.
           */
          std::time_t creation_time_;


          /**
           * Length of the unique alphanumeric id assigned to the client when created.
           */
          int client_id_length_;


          /**
           * @override
           * Loads properties given in the configuration file, if properties
           * are not found, then default values included in granada/defaults.dat
           * file are used.
           */
          virtual void LoadProperties();


          /**
           * @override
           * Returns the key of the client values : that is the namespace and the client id
           * Example:
           * 			oauth2.client:value:myfNv849Z1GNuPAN
           * This is the key to retrieve the client values, such as its key, type, redirect URIs,
           * roles, creation time.
           * @return Key of the client values.
           */
          virtual std::string hash(){
            return cache_namespace_ + id_;
          };

      };


      /**
       * OAuth 2.0 User
       * The user is the resource owner and the person that authorizes or denies the client to access the user's resources.
       * Our authorization server must know the user. OAuth 2.0 User should not be used for accessing resources,
       * this is used for authorization an authentication purposes.
       */
      class OAuth2User : public OAuth2Entity{

        public:

          /**
           * Constructor
           * Loads the properties.
           */
          OAuth2User();


          /**
           * Constructor
           * Loads the properties and the values of the user with given username.
           * @param username Username.
           */
          OAuth2User(const std::string& username);


          /**
           * @override
           * Loads the values of the user retrieving them from the cache
           * with the username.
           */
          virtual void Load();

          /**
           * @override
           * Loads the values of the user retrieving them from the cache
           * with the given username.
           * @param identifier Username.
           */
          virtual void Load(const std::string& identifier);


          /**
           * Creates an OAuth 2.0 user. This user should not be used for accessing resources,
           * this is used for authorization and authentication purposes.
           * It will be stored in the cache with a key like: oauth2.user:value:username.
           * @param  username Username.
           * @param  password Password of the user.
           * @param  roles    Maximum roles a user can be authorize to authorize a client to have,
           *                  roles manage the permissions an entity has over user's resources.
           * @return          True if client is successfully, false if it has not because it already exists.
           */
          virtual bool Create(const std::string& username, std::string& password, const web::json::value& roles);


          /**
           * Verify if user credentials are correct. Returns true if they are,
           * and false if they are not.
           * @param  username Username
           * @param  password User password
           * @return          True if credentials are correct and false if they are not.
           */
          virtual bool CorrectCredentials(std::string& password);


          /**
           * Delete a user.
           * @param  password User password.
           * @return          True if user has successfuly been deleted.
           */
          virtual bool Delete(const std::string& password);


          /**
           * Returns true if user has the given role in its collection.
           * False if it does not.
           * @param  role Role to check.
           * @return      True if user has the given role in its collection, false if it does not.
           */
          virtual const bool HasRole(const std::string& role){
            return roles_.is_null()? false : roles_.has_field(role);
          };


          virtual const std::string GetUsername(){
            return username_;
          };

          virtual void SetUsername(const std::string& username){
            username_.assign(username);
          };

          virtual web::json::value GetRoles(){
            return roles_;
          };

          virtual const std::time_t GetCreationTime(){
            return creation_time_;
          };


        protected:

          /**
           * Username. Unique identifier of the user.
           */
          std::string username_;


          /**
           * Crypted key, used with the user password to verify the client credentials
           * when needed.
           */
          std::string key_;


          /**
           * Maximum roles a user can be authorize to authorize a client to have,
           * roles manage the permissions an entity has over user's resources.
           */
          web::json::value roles_;


          /**
           * Date of the user creation.
           */
          std::time_t creation_time_;


          /**
           * @override
           * Loads properties given in the configuration file, if properties
           * are not found, then default values included in granada/defaults.dat
           * file are used.
           */
          virtual void LoadProperties();


          /**
           * @override
           * Returns the key of the user values : that is the namespace and the username
           * Example:
           * 			oauth2.user:value:johndoe
           * This is the key to retrieve the user values, such as its key, roles, creation time.
           * @return Key of the OAuth 2.0 user values.
           */
          virtual std::string hash(){
            return cache_namespace_ + username_;
          };

      };


      /**
       * OAuth 2.0 Code
       * The authorization code is obtained by using an authorization server
       * as an intermediary between the client and resource owner.  Instead of
       * requesting authorization directly from the resource owner, the client
       * directs the resource owner to an authorization server (via its
       * user-agent as defined in [RFC2616]), which in turn directs the
       * resource owner back to the client with the authorization code.
       *
       * Before directing the resource owner back to the client with the
       * authorization code, the authorization server authenticates the
       * resource owner and obtains authorization.  Because the resource owner
       * only authenticates with the authorization server, the resource
       * owner's credentials are never shared with the client.
       *
       * The authorization code provides a few important security benefits,
       * such as the ability to authenticate the client, as well as the
       * transmission of the access token directly to the client without
       * passing it through the resource owner's user-agent and potentially
       * exposing it to others, including the resource owner.
       */
      class OAuth2Code : public OAuth2Entity{

        public:

          /**
           * Constructor
           * Loads the properties.
           */
          OAuth2Code();


          /**
           * Constructor
           * Loads the properties and the values of the code with given code.
           * @param code Unique alphanumeric code.
           */
          OAuth2Code(const std::string& code);


          /**
           * @override
           * Loads the values of the code retrieving them from the cache
           * with the code.
           */
          virtual void Load();


          /**
           * @override
           * Loads the values of the code retrieving them from the cache
           * with the given code.
           * @param identifier Unique alphanumeric code.
           */
          virtual void Load(const std::string& identifier);


          /**
           * Creates an OAuth 2.0 user. This user should not be used for accessing resources,
           * this is used for authorization and authentication purposes.
           * It will be stored in the cache with a key like: oauth2.user:value:username.
           * @param client_id [description]
           * @param roles     [description]
           * @param username  [description]
           */
          virtual void Create(const std::string& client_id, const std::string& roles, const std::string& username);


          /**
           * Delete code from where it is stored.
           */
          virtual void Delete();


          virtual const std::string GetCode(){
            return code_;
          };

          virtual void SetCode(const std::string& code){
            code_.assign(code);
          };

          virtual const std::string GetClientId(){
            return client_id_;
          };

          virtual const std::string GetUsername(){
            return username_;
          };

          virtual const std::vector<std::string> GetRoles(){
            return roles_;
          };

          virtual const std::time_t GetCreationTime(){
            return creation_time_;
          };

        protected:

          /**
           * Alphanumeric unique code.
           */
          std::string code_;


          /**
           * Client id. Unique alphanumeric hash identifying the client.
           */
          std::string client_id_;


          /**
           * Username. Unique user identifier.
           */
          std::string username_;


          /**
           * Maximum roles a user can be authorize to authorize a client to have,
           * roles manage the permissions an entity has over user's resources.
           */
          std::vector<std::string> roles_;


          /**
           * Date when code is created.
           */
          std::time_t creation_time_;


          /**
           * Length of the unique alphanumeric code.
           */
          int code_length_;


          /**
           * @override
           * Loads properties given in the configuration file, if properties
           * are not found, then default values included in granada/defaults.dat
           * file are used.
           */
          virtual void LoadProperties();


          /**
           * @override
           * Returns the key of the code values : that is the namespace and the code
           * Example:
           * 			oauth2.code:value:RQlUIgDxgCaTO8CLhAxNzveNtCHPQFrf
           * This is the key to retrieve the code values, such as its client_id, username,
           * roles, creation time.
           * @return Key of the OAuth 2.0 code values.
           */
          virtual std::string hash(){
            return cache_namespace_ + code_;
          };

      };

      /**
       * Authorize a client.
       * Authorization Code Grant and Implicit Grant.
       */
      class OAuth2Authorization : public OAuth2Entity{
        public:

          /**
           * Constructor
           * Load properties.
           */
          OAuth2Authorization();

          /**
           * Constructor
           * Load properties.
           */
          OAuth2Authorization(const granada::http::oauth2::OAuth2Parameters& oauth2_parameters,
                              std::shared_ptr<granada::http::session::Checkpoint>& session_checkpoint,
                              std::shared_ptr<granada::http::oauth2::OAuth2Factory>& oauth2_factory);


          /**
           * Process Grant code authorization, Implicit grant, access token request.
           * @param  request  HTTP request.
           * @param  response HTTP response;
           * @return          OAuth 2.0 parameters containing the response: error, code or access token.
           */
          virtual granada::http::oauth2::OAuth2Parameters Grant(web::http::http_request &request, web::http::http_response& response);


          /**
           * Returns information about the clients authorized by a given user
           * or the codes used by a client to obtain access_tokens. The username and
           * the client_id are taken from the oauth2_parameters_ member.
           * @return JSON containing an array with the clients authorized by a user
           *         or a list with the codes used by a client.
           */
          virtual web::json::value Information();


          /**
           * Delete the authorization given by a user to a client by deleting
           * the codes and access_tokens and closing the sessions used by the
           * client to access the resources of the user.
           * @return Returns the return JSON of the Information() function.
           */
          virtual web::json::value Delete();


        protected:

          /**
           * Session check point. Allows to have a unique point for checking and setting sessions.
           * It is used to create a new session without knowing its type.
           * Used to create OAuth 2.0 user sessions and OAuth 2.0 client sessions.
           */
          std::shared_ptr<granada::http::session::Checkpoint> session_checkpoint_;


          /**
           * OAuth 2.0 Factory.
           * Used to instanciate OAuth 2.0 clients, users and codes.
           */
          std::shared_ptr<granada::http::oauth2::OAuth2Factory> oauth2_factory_;


          /**
           * OAuth 2.0 Parameters.
           * Parameters for use with the authorization endpoint,
           * the token endpoint and the information and deletion endpoint.
           */
          granada::http::oauth2::OAuth2Parameters oauth2_parameters_;


          /**
           * If true when client request an access token a refresh token is also delivered.
           * The refresh token can be used to obtain new access tokens using the same
           * authorization grant.
           */
          bool oauth2_use_refresh_token_;


          /**
           * @override
           * Loads properties given in the configuration file, if properties
           * are not found, then default values included in granada/defaults.dat
           * file are used.
           */
          virtual void LoadProperties();


          /**
           * @override
           * Returns the key made with the user, the client, the code and the session identifiers.
           * This key is used to search the clients authorized by a user and their codes/tokens
           * and consequently limit the clients authorizations when the user decides.
           * Examples:
           * 		Authorization Code Grant:
           * 			oauth2.authorization:johndoe:gida8fZEFh9abpkg:Gkt2DkEv94jXLhOV7ezd8tdTro2qwOnjNM30hAAJrNPDllUBnzk9cxsIfMA1ecsY:Gkt2DkEv94jXLhOV7ezd8tdTro2qwOnjNM30hAAJrNPDllUBnzk9cxsIfMA1ecs52
           * 			oauth2.authorization:johndoe:gida8fZEFh9abpkg:Gkt2DkEv94jXLhOV7ezd8tdTro2qwOnjNM30hAAJrNPDllUBnzk9cxsIfMA1ecsY:
           * 		Implicit Grant:
           * 			oauth2.authorization:johndoe:gida8fZEFh9abpkg::Gkt2DkEv94jXLhOV7ezd8tdTro2qwOnjNM30hAAJrNPDllUBnzk9cxsIfMA1ecs52
           * @return Key made with the user, the client, the code and the session identifiers.
           */
          virtual std::string hash(){
            return cache_namespace_ + oauth2_parameters_.username + ":" + oauth2_parameters_.client_id + ":" + oauth2_parameters_.code + ":" + oauth2_parameters_.access_token;
          };


          /**
           * Checks the validity of a client based on the client URI and the client id.
           * If something is wrong explicit it in oauth2_response filling the error and
           * the error_description members.
           *
           * @param oauth2_client     OAuth 2.0 client.
           * @param oauth2_response   OAuth 2.0 parameters.
           */
          virtual void CheckClient(std::shared_ptr<granada::http::oauth2::OAuth2Client>& oauth2_client, granada::http::oauth2::OAuth2Parameters& oauth2_response);


          /**
           * Checks the validity of the provided code, client credentials
           * or user credentials, depending on the grant type and the
           * provided credentials.
           * If something is wrong explicit it in oauth2_response filling the error and
           * the error_description members.
           *
           * @param oauth2_client       OAuth 2.0 client. Used in case we need to check client credentials.
           * @param oauth2_user         OAuth 2.0 user. Used in case we need to check user credential.
           * @param oauth2_code         OAuth 2.0 code. Used in case a code has been provided and we need to
           *                            check its validity.
           * @param oauth2_user_session Session. In case the user has already provided his credentials before,
           *                            he is already "logged" in our system, we don't try to validate his credentials,
           *                            instead we check that his session is valid and retrieve the username from the
           *                            session properties.
           * @param oauth2_response     OAuth 2.0 parameters containing the response: error, code or access token.
           * @param request             HTTP request.
           * @param response            HTTP response.
           */
          virtual void CheckCredentials(std::shared_ptr<granada::http::oauth2::OAuth2Client>& oauth2_client,
                                         std::shared_ptr<granada::http::oauth2::OAuth2User>& oauth2_user,
                                         std::shared_ptr<granada::http::oauth2::OAuth2Code>& oauth2_code,
                                         std::shared_ptr<granada::http::session::Session>& oauth2_user_session,
                                         granada::http::oauth2::OAuth2Parameters& oauth2_response,
                                         web::http::http_request& request,
                                         web::http::http_response& response);


          /**
           * Creates an OAuth 2.0 unique code for a client so it can then use it to
           * request an acces_token to have access to user's resources.
           * If something goes wrong explicit it in oauth2_response filling the error and
           * the error_description members.
           *
           * @param oauth2_user_session The session of the user logged in the authorization server.
           * @param oauth2_code         OAuth 2.0 code. Will be filled with new data.
           * @param oauth2_user         OAuth 2.0 user. Used to get the roles of a user to store
           *                            them as a maximum scope in his session.
           * @param oauth2_response     OAuth 2.0 parameters containing the response: error or generated code.
           * @param request             HTTP request.
           * @param response            HTTP response.
           */
          virtual void CreateCode(std::shared_ptr<granada::http::session::Session>& oauth2_user_session,
                                   std::shared_ptr<granada::http::oauth2::OAuth2Code>& oauth2_code,
                                   std::shared_ptr<granada::http::oauth2::OAuth2User>& oauth2_user,
                                   granada::http::oauth2::OAuth2Parameters& oauth2_response,
                                   web::http::http_request& request,
                                   web::http::http_response& response);


          /**
           * Creates a token that permits to have a limited access to some user's ressources.
           * If something goes wrong explicit it in oauth2_response filling the error and
           * the error_description members.
           *
           * @param roles               Roles we want to give to the "token" (in fact it will be
           *                            the session that will hold the roles not the token).
           * @param oauth2_user_session The session of the user logged in the authorization server.
           * @param oauth2_user         OAuth 2.0 user. Used to get the roles of a user to store
           *                            them as a maximum scope in his session. Only in case of
           *                            an implicit grant.
           * @param oauth2_code         Auth 2.0 code. Code that will be used in case we want to create
           *                            a refresh token. (Refresh tokens are in our case the same as an
           *                            OAuth 2.0 code).
           * @param oauth2_response     OAuth 2.0 parameters containing the response: error or generated code.
           * @param request             HTTP request.
           * @param response            HTTP response.
           */
          virtual void CreateAccessToken(std::vector<std::string>& roles,
                                          std::shared_ptr<granada::http::session::Session>& oauth2_user_session,
                                          std::shared_ptr<granada::http::oauth2::OAuth2User>& oauth2_user,
                                          std::shared_ptr<granada::http::oauth2::OAuth2Code>& oauth2_code,
                                          granada::http::oauth2::OAuth2Parameters& oauth2_response,
                                          web::http::http_request& request,
                                          web::http::http_response& response);


          /**
           * Creates a refresh token. The refresh token can be used to obtain new access tokens using the same
           * authorization grant.
           * @param oauth2_client_session OAuth 2.0 client session created when creating an access token.
           *                              So it is the session that allows the client to access the user's resources.
           * @param oauth2_code           It is the refresh token. Refresh tokens are in our case the same as an
           *                              OAuth 2.0 code
           * @param oauth2_response       OAuth 2.0 parameters containing the response: error or generated code.
           */
          virtual void CreateRefreshToken(std::shared_ptr<granada::http::session::Session>& oauth2_client_session,
                                          std::shared_ptr<granada::http::oauth2::OAuth2Code>& oauth2_code,
                                          granada::http::oauth2::OAuth2Parameters& oauth2_response);


          /**
           * Checks if the code or token requested are allowed to have the asked scope or level
           * of acces to the user's resources. Talking about server sessions: if they are allowed
           * to have the asked roles. This function checks if the client is allowed to have the asked
           * role AND if the user is allowed to give the client the authorization to have the asked
           * role.
           * @param  roles         Roles manage permissions over a certain resource. Example: "msg.insert" role
           *                       allows the owner of the session to create user's messages.
           * @param  oauth2_client OAuth 2.0 client.
           * @param  oauth2_user   Oauth 2.0 user.
           * @return               True if the client is allowed to have the asked roles and the user can
           *                       authorize the client to have them.
           */
          virtual bool CheckRoleAllowance(std::vector<std::string>& roles,
                                           std::shared_ptr<granada::http::oauth2::OAuth2Client>& oauth2_client,
                                           std::shared_ptr<granada::http::oauth2::OAuth2User>& oauth2_user);


          /**
           * Assign to a client session a number of roles and also fill the properties of the roles based
           * on the user's roles properties.
           * @param roles                 Roles manage permissions over a certain resource. Example: "msg.insert" role
           *                              allows the owner of the session to create user's messages.
           * @param oauth2_user           Oauth 2.0 user.
           * @param oauth2_client_session Session of the OAuth 2.0 client used to access to the user's resources.
           */
          virtual void AssignRolesToClientSession(std::vector<std::string>& roles,
                                                   std::shared_ptr<granada::http::oauth2::OAuth2User>& oauth2_user,
                                                   std::shared_ptr<granada::http::session::Session>& oauth2_client_session);


          /**
           * Once a user logs into the authorization server a session is created, this session is used so the
           * user does not have to loggin each time a client request an authorization over his resources. This
           * session has to contain the user's roles (permissions) and it's properties so we can then give them
           * to the client if requested.
           * @param oauth2_user_session The session of the user logged in the authorization server.
           * @param oauth2_user         OAuth 2.0 user. Used to get the roles and its properties and give them
           *                            to the session.
           * @param request             HTTP request.
           * @param response            HTTP response.
           */
          virtual void AssignRolesToOAuth2UserSession(std::shared_ptr<granada::http::session::Session>& oauth2_user_session,
                                                               std::shared_ptr<granada::http::oauth2::OAuth2User>& oauth2_user,
                                                               web::http::http_request& request,
                                                               web::http::http_response& response);

      };


      /**
       * OAuth 2.0 Factory
       * Used to instanciate OAuth 2.0 clients, users and codes.
       */
      class OAuth2Factory{

        public:

          virtual std::shared_ptr<granada::http::oauth2::OAuth2Client>OAuth2Client(){
            return std::shared_ptr<granada::http::oauth2::OAuth2Client>(new granada::http::oauth2::OAuth2Client());
          };

          virtual std::shared_ptr<granada::http::oauth2::OAuth2Client>OAuth2Client(const std::string& client_id){
            return std::shared_ptr<granada::http::oauth2::OAuth2Client>(new granada::http::oauth2::OAuth2Client(client_id));
          };

          virtual std::shared_ptr<granada::http::oauth2::OAuth2User>OAuth2User(){
            return std::shared_ptr<granada::http::oauth2::OAuth2User>(new granada::http::oauth2::OAuth2User());
          };

          virtual std::shared_ptr<granada::http::oauth2::OAuth2User>OAuth2User(const std::string& username){
            return std::shared_ptr<granada::http::oauth2::OAuth2User>(new granada::http::oauth2::OAuth2User(username));
          };

          virtual std::shared_ptr<granada::http::oauth2::OAuth2Code>OAuth2Code(){
            return std::shared_ptr<granada::http::oauth2::OAuth2Code>(new granada::http::oauth2::OAuth2Code());
          };

          virtual std::shared_ptr<granada::http::oauth2::OAuth2Code>OAuth2Code(const std::string& code){
            return std::shared_ptr<granada::http::oauth2::OAuth2Code>(new granada::http::oauth2::OAuth2Code(code));
          };

          virtual std::shared_ptr<granada::http::oauth2::OAuth2Authorization>OAuth2Authorization(){
            return std::shared_ptr<granada::http::oauth2::OAuth2Authorization>(new granada::http::oauth2::OAuth2Authorization());
          };

          virtual std::shared_ptr<granada::http::oauth2::OAuth2Authorization>OAuth2Authorization(const granada::http::oauth2::OAuth2Parameters& oauth2_parameters,
                                                                                                 std::shared_ptr<granada::http::session::Checkpoint>& session_checkpoint,
                                                                                                 std::shared_ptr<granada::http::oauth2::OAuth2Factory>& oauth2_factory){
            return std::shared_ptr<granada::http::oauth2::OAuth2Authorization>(new granada::http::oauth2::OAuth2Authorization(oauth2_parameters,session_checkpoint,oauth2_factory));
          };

      };
    }
  }
}
