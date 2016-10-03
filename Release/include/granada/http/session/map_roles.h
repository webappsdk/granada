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
  * Manage the roles of a session and their properties.
  *
  */
#pragma once
#include "roles.h"
#include <unordered_map>

namespace granada{
  namespace http{
    namespace session{
      class MapRoles : public Roles
      {
        public:

          /**
           * Controlers
           */
          MapRoles(granada::http::session::Session* session);

          // override
          const bool Is(const std::string& role_name);


          // override
          const bool Add(const std::string& role_name);


          // override
          void Remove(const std::string& role_name);


          // override
          void RemoveAll();


          // override
          void SetProperty(const std::string& role_name, const std::string& key, const std::string& value);


          // override
          const std::string GetProperty(const std::string& role_name, const std::string& key);


          // override
          void DestroyProperty(const std::string& role_name, const std::string& key);

        private:

          /**
           * Unordered map containing the roles and their properties.
           * Example:
           * 	roles
           * 			|_ ANONYMOUS (role name)
           * 	  	|_ USER (role name)
           * 	  				|_ username : user46348 (property)
           */
          std::unordered_map<std::string, std::unordered_map<std::string, std::string>> roles_and_properties_;
      };
    }
  }
}
