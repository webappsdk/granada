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
  * Manages the cache
  */

#include "granada/cache/shared_map_cache_driver.h"

namespace granada{
  namespace cache{

    SharedMapCacheDriver::SharedMapCacheDriver(){
      data_ = std::unique_ptr<std::map<std::string,std::unordered_map<std::string,std::string>>>(new std::map<std::string,std::unordered_map<std::string,std::string>>);
    }

    std::string SharedMapCacheDriver::Read(const std::string& hash,const std::string& key){
      mtx.lock();
      auto it = data_->find(hash);
      if (it != data_->end()){
        std::unordered_map<std::string,std::string> properties = it->second;
        auto it2 = properties.find(key);
        if(it2 != properties.end()){
          mtx.unlock();
          return it2->second;
        }
      }
      mtx.unlock();
      return std::string();
    }

    std::unordered_map<std::string,std::string> SharedMapCacheDriver::GetProperties(const std::string& hash){
      mtx.lock();
      auto it = data_->find(hash);
      if (it != data_->end()){
        mtx.unlock();
        return it->second;
      }
      mtx.unlock();
      return std::unordered_map<std::string,std::string>();
    }

    void SharedMapCacheDriver::Write(const std::string& hash,const std::string& key,const std::string& value){
      mtx.lock();
      auto it = data_->find(hash);
      if (it == data_->end()){
        std::unordered_map<std::string,std::string> properties;
        properties[key] = value;
        (*data_)[hash] = properties;
      }else{
        std::unordered_map<std::string,std::string> properties = it->second;
        properties[key] = value;
        (*data_)[hash] = properties;
      }
      mtx.unlock();
    }

    void SharedMapCacheDriver::Write(const std::string& hash,const std::unordered_map<std::string,std::string>& properties){
      mtx.lock();
      (*data_)[hash] = properties;
      mtx.unlock();
    }

    void SharedMapCacheDriver::Destroy(const std::string& hash){
      mtx.lock();
      data_->erase(hash);
      mtx.unlock();
    }

    void SharedMapCacheDriver::Destroy(const std::string& hash,const std::string& key){
      mtx.lock();
      auto it = data_->find(hash);
      if (it != data_->end()){
        std::unordered_map<std::string,std::string> properties = it->second;
        properties.erase(key);
        (*data_)[hash] = properties;
      }
      mtx.unlock();
    }

    int SharedMapCacheDriver::Length(const std::string& hash){
      return GetProperties(hash).size();
    }
  }
}
