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
  * Manages the cache storing key-value pairs in a map.
  * This code is not multi-thread safe, If you want to use a shared map
  * between all the users cache, We recommend to use
  * shared_map_cache_driver instead.
  *
  */
#pragma once
#include "cache_handler.h"
#include <regex>
#include <string>
#include <map>
#include <deque>
#include "granada/util/string.h"

namespace granada{
  namespace cache{

    class MapCacheDriver;

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
         * Constructor.
         * @param expression    Expression used to match keys.
         *                    
         *                      Example of expression:
         *                        
         *                        session:value:*
         *                        => will retrieve all the keys that start with
         *                        session:values: stored in the cache.
         *                        
         *                        *:value:*
         *                        => will retrieve all the keys that contain
         *                        ":value:"
         *                        
         * @param cache         Pointer to the cache where to search the keys.
         */
        MapIterator(const std::string& expression, MapCacheDriver* cache);


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
         * Vector for storing found keys.
         */
        std::vector<std::string> keys_;


        /**
         * Iterator.
         */
        std::vector<std::string>::iterator it_;

    };

    class MapCacheDriver : public CacheHandler
    {
      public:

        /**
         * Constructor
         */
        MapCacheDriver();


        /**
         * Checks if a key exist in the cache.
         * @param  key  Key to check.
         */
        const bool Exists(const std::string& key);


        /**
         * Returns value from the cache.
         * @param  key Key of the value.
         * @return     Value
         */
        const std::string Read(const std::string& key);


        /**
         * Set a value in the cache associated with a given key.
         * @param key   Key of the value.
         * @param value Value.
         */
        void Write(const std::string& key,const std::string& value);


        /**
         * Inserts or rewrite a key-value pair in a set with the given name.
         * If the set does not exist, it creates it.
         * @param hash  Name of the set.
         * @param key   Key to identify the value inside the set.
         * @param value Value
         */
        void Destroy(const std::string& key);


        /**
         * Fills a vector with keys of the cache that match
         * a given expression.
         * 
         * @param expression  Expression used to match keys.
         *                    
         *                    Example of expression:
         *                        
         *                        session:value:*
         *                        => will retrieve all the keys that start with
         *                        session:values: stored in the cache.
         *                        
         *                        *:value:*
         *                        => will retrieve all the keys that contain
         *                        ":value:"
         *                         
         * @return            Vector of string keys.
         */
        void Keys(const std::string& expression, std::vector<std::string>& keys);


        /**
         * Returns an iterator to iterate over keys with an expression.
         * @param   Expression to be use to iterate over keys that match this expression.
         *          Example: "user*" => we will iterate over all the keys that start with "user"
         * @return  Iterator.
         */
        std::shared_ptr<granada::cache::CacheHandlerIterator> make_iterator(const std::string& expression){
          return std::shared_ptr<granada::cache::CacheHandlerIterator>(new granada::cache::MapIterator(expression,this));
        };

      protected:

        /**
         * Unordered map where key-value pairs are stored.
         */
        std::shared_ptr<std::map<std::string,std::string>> data_;

    };
  }
}
