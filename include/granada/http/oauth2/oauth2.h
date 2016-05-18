#pragma once

#include <string>
#include <unordered_map>
#include "cpprest/json.h"
#include "cpprest/http_msg.h"
#include "cpprest/oauth2.h"
#include "granada/http/parser.h"

// Constant strings for OAuth 2.0.
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


using namespace web::http::oauth2::details;

namespace granada{

  namespace http{
    /**
     * OAuth2 Parameters and parsing functions
     * Example:
     * 		content_type     => text/javascript; charset=utf-8
     * 		content_encoding => gzip
     * 		content          => console.log("content of a javascript resource.");
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
         */
        OAuth2Parameters(const std::string& query_string){
          try{
            std::unordered_map<std::string, std::string>  parsed_data =	granada::http::parser::ParseQueryString(query_string);
            username.assign(parsed_data["username"]);
            password.assign(parsed_data["password"]);
            code.assign(parsed_data[oauth2_strings::code]);
            access_token.assign(parsed_data[oauth2_strings::access_token]);
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


        std::unordered_map<std::string,std::string> to_unordered_map(){
          std::unordered_map<std::string,std::string> map;
          if (!code.empty()){ map.insert(std::make_pair(oauth2_strings::code,code)); }
          if (!access_token.empty()){ map.insert(std::make_pair(oauth2_strings::access_token,access_token)); }
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

        std::string to_query_string(){
          std::string query_string = "";
          if (!code.empty()){ query_string+="&"+oauth2_strings::code+"="+code; }
          if (!access_token.empty()){ query_string+="&"+oauth2_strings::access_token+"="+access_token; }
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


        web::json::value to_json(){
          std::string json_str = "";
          if (!code.empty()){ json_str+=",\""+oauth2_strings::code+"\":\""+code+"\""; }
          if (!access_token.empty()){ json_str+=",\""+oauth2_strings::access_token+"\":\""+access_token+"\""; }
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
        std::string access_token;
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
  }
}
