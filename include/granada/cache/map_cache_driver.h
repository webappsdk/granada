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
  * Manages the cache storing key-value pairs in an unordered map.
  * This code is not multi-thread safe, If you want to use a shared map
  * between all the users cache, We recommend to use
  * shared_map_cache_driver instead.
  *
  */
#pragma once
#include "cache_handler.h"
#include <regex>
#include <string>
#include <unordered_map>
#include "granada/util/string.h"

namespace granada{
  namespace cache{

    /**
     * Tool for iterate over keys with a given pattern.
     */
    class MapIterator : public CacheHandlerIterator{
      public:
        /**
         * Constructor
         */
        MapIterator(){};


        /**
         * Constructor
         */
        MapIterator(const std::string& expression, std::shared_ptr<std::unordered_map<std::string,std::string>>& data);


        /**
         * Return true if there is another value with same pattern, false
         * if there is not.
         * @return True | False
         */
        const bool has_next();


        /**
         * Return the next key found with the given pattern.
         * @return [description]
         */
        const std::string next();


      protected:

        /**
         * Map where all data is store.
         */
        std::shared_ptr<std::unordered_map<std::string,std::string>> data_;


        /**
         * Map where all data is store.
         */
        std::vector<std::string> keys_;


        std::vector<std::string>::iterator it_;


        void FilterKeys();

    };

    class MapCacheDriver : public CacheHandler
    {
      public:

        /**
         * Constructor
         */
        MapCacheDriver();


        // override
        const bool Exists(const std::string& key);


        // override
        const std::string Read(const std::string& key);


        // override
        void Write(const std::string& key,const std::string& value);


        // override
        void Destroy(const std::string& key);


        /**
         * Returns an iterator to iterate over keys with an expression.
         * @param   Expression to be use to iterate over keys that match this expression.
         *          Example: "user*" => we will iterate over all the keys that start with "user"
         * @return  Iterator.
         */
        std::shared_ptr<granada::cache::CacheHandlerIterator> make_iterator(const std::string& expression){
          return std::shared_ptr<granada::cache::CacheHandlerIterator>(new granada::cache::MapIterator(expression,data_));
        };

      protected:

        /**
         * Unordered map where key-value pairs are stored.
         */
        std::shared_ptr<std::unordered_map<std::string,std::string>> data_;

    };
  }
}
