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
  */

#include "granada/http/session/map_roles.h"

namespace granada{
  namespace http{
    namespace session{

      MapRoles::MapRoles(granada::http::session::Session* session){
        session_ = session;
      }

      const bool MapRoles::Is(const std::string& role_name){
        auto it = roles_and_properties_.find(role_name);
        if (it != roles_and_properties_.end()){
          return true;
        }
        return false;
      }

      const bool MapRoles::Add(const std::string& role_name){
        // add only if role is not already added.
        if (!Is(role_name)){
          std::unordered_map<std::string, std::string> properties;
          roles_and_properties_.insert(std::make_pair(role_name,properties));
          // update session, changes are saved wherever sessions are stored.
          session_->Update();
          return true;
        }
        return false;
      }

      void MapRoles::Remove(const std::string& role_name){
        roles_and_properties_.erase(role_name);
        // update session, changes are saved wherever sessions are stored.
        session_->Update();
      }

      void MapRoles::RemoveAll(){
        roles_and_properties_.clear();
      };

      void MapRoles::SetProperty(const std::string& role_name, const std::string& key, const std::string& value){
        // set property only if role exists.
        auto it = roles_and_properties_.find(role_name);
        if (it != roles_and_properties_.end()){
          it->second[key] = value;
          // update session, changes are saved wherever sessions are stored.
          session_->Update();
        }
      }

      const std::string MapRoles::GetProperty(const std::string& role_name, const std::string& key){
        auto it = roles_and_properties_.find(role_name);
        if (it != roles_and_properties_.end()){
          std::unordered_map<std::string, std::string> properties = it->second;
          auto it2 = properties.find(key);
          if (it2 != properties.end()){
            return it2->second;
          }
        }
        return std::string();
      }

      void MapRoles::DestroyProperty(const std::string& role_name, const std::string& key){
        auto it = roles_and_properties_.find(role_name);
        if (it != roles_and_properties_.end()){
          it->second.erase(key);
          // update session, changes are saved wherever sessions are stored.
          session_->Update();
        }
      }
    }
  }
}
