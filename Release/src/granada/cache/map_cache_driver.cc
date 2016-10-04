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

#include "granada/cache/map_cache_driver.h"

namespace granada{
  namespace cache{

    MapIterator::MapIterator(const std::string& expression, MapCacheDriver* cache){
      expression_ = expression;
      std::deque<std::pair<std::string,std::string>> values;
      values.push_back(std::make_pair("*",".*"));
      granada::util::string::replace(expression_,values,"","");
      cache->Keys(expression_,keys_);
      it_ = keys_.begin();
    }

    const bool MapIterator::has_next(){
      return it_ != keys_.end();
    }

    const std::string MapIterator::next(){
      if (it_ != keys_.end()){
        std::string value = *it_;
        ++it_;
        return value;
      }
      return std::string();
    }


    MapCacheDriver::MapCacheDriver(){
      data_ = std::shared_ptr<std::map<std::string,std::string>>(new std::map<std::string,std::string>());
    }

    const bool MapCacheDriver::Exists(const std::string& key){
      auto it = data_->find(key);
      if (it != data_->end()){
        return true;
      }
      return false;
    }

    const std::string MapCacheDriver::Read(const std::string& key){
      auto it = data_->find(key);
      if (it != data_->end()){
        return it->second;
      }
      return std::string();
    }

    void MapCacheDriver::Write(const std::string& key,const std::string& value){
      (*data_)[key] = value;
    }

    void MapCacheDriver::Destroy(const std::string& key){
      std::size_t found = key.find("*");
      if (found!=std::string::npos){
        std::vector<std::string> keys;
        Match(key,keys);
        for (auto it = keys.begin(); it != keys.end(); ++it){
          data_->erase(*it);
        }
      }else{
        data_->erase(key);
      }
    }

    void MapCacheDriver::Keys(const std::string& expression, std::vector<std::string>& keys){
      keys.clear();
      std::string key;
      for(auto it = data_->begin(); it != data_->end(); ++it) {
        key = it->first;
        if (std::regex_match(key, std::regex(expression))){
          keys.push_back(it->first);
        }
      }
    }
  }
}
