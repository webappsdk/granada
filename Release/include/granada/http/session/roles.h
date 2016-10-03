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
  * Interface for managers of the sessions' roles and their properties.
  * Example of roles:
  * 	roles
  * 			|_ ANONYMOUS (role name)
  * 	  	|_ USER (role name)
  * 	  				|_ username : user46348 (property)
  *
  */
#pragma once
#include <string>
#include "session.h"

namespace granada{
  namespace http{
    namespace session{

      class Session;
      class Roles
      {
        public:

          /**
           * Constructors
           */
          Roles(){};

          Roles(granada::http::session::Session* session);


          /**
           * Destructor
           */
          virtual ~Roles(){};


          virtual void SetSession(granada::http::session::Session* session){
            session_ = session;
          }


          /**
           * Check if the role with the given name is stored.
           * Returns true if the role is found or false if it does not.
           * @param  role_name Name of the role to check.
           * @return           true | false
           */
          virtual const bool Is(const std::string& role_name){ return false; };


          /**
           * Add a new role if it has not already been added.
           * @param  role_name Name of the role.
           * @return           True if role added correctly, false if the role
           *                   has not been added, for example because the role
           *                   is already added.
           */
          virtual const bool Add(const std::string& role_name){ return false; };


          /**
           * Remove a role.
           * @param role_name Name of the role.
           */
          virtual void Remove(const std::string& role_name){};


          /**
           * Remove all roles.
           */
          virtual void RemoveAll(){};


          /**
           * Set a role property, it has to be a string.
           * @param role_name Role name
           * @param key       Key or name of the property.
           * @param value     Value of the property.
           */
          virtual void SetProperty(const std::string& role_name, const std::string& key, const std::string& value){};


          /**
           * Get a role property, returns a string.
           * @param  role_name Role name.
           * @param  key       Key or name of the property.
           * @return           Value of the property (string).
           */
          virtual const std::string GetProperty(const std::string& role_name, const std::string& key){ return std::string(); };


          /**
           * Remove a role property.
           * @param role_name Role name.
           * @param key       Key or name of the property.
           */
          virtual void DestroyProperty(const std::string& role_name, const std::string& key){};




        protected:

          /**
           * Pointer of the session, owner of the roles.
           */
          granada::http::session::Session* session_;
      };
    }
  }
}
