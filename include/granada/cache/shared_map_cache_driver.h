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
  * Manages the cache storing key-value pairs in unordered maps.
  * map
  * 	|_ hash1 => unordered_map
  * 								|_ key1 => value1
  * 							 	|_ key2 => value2
  * 	|_ hash2 => unordered_map
  * 								|_ key1 => value3
  * 							 	|_ key2 => value4
  *
  * This code is multi-thread safe.
  *
  */

#pragma once
#include <memory>
#include <mutex>
#include "cache_handler.h"
#include <string>
#include <map>
#include <unordered_map>

namespace granada{
  namespace cache{
    class SharedMapCacheDriver : public CacheHandler
    {
      public:

        /**
         * Constructor
         */
        SharedMapCacheDriver();


        /**
         * Returns the value of a key-value pair stored in
         * an unordered_map with the given name.
         * @param  hash Name of the unordered map.
         * @param  key  Key to identify the value.
         * @return      Value.
         */
        const std::string Read(const std::string& hash,const std::string& key);


        /**
         * Returns an unordered map with given name containing key-value pairs.
         */
        const std::unordered_map<std::string,std::string> GetProperties(const std::string& hash);


        /**
         * Inserts or rewrite a key-value pair in an unordered_map with the given name.
         * If the set does not exist, it creates it.
         * @param  hash Name of the unordered map.
         * @param  key  Key to identify the value.
         * @param       Value.
         */
        void Write(const std::string& hash,const std::string& key,const std::string& value);


        /**
         * Inserts or rewrite an unordered map of key-value pairs
         * @param  hash       Name of the unordered map.
         * @param  properties Unordered map with key-value pairs.
         */
        void Write(const std::string& hash,const std::unordered_map<std::string,std::string>& properties);


        /**
         * Destroys a set of key-value pairs with the given name.
         * @param hash Name of the unordered map containing the key-value pairs
         */
        void Destroy(const std::string& hash);


        /**
         * Destroy a key value pair of a given set.
         * @param hash Name of the unordered map containing the key-value pair to destroy.
         * @param key  Key associated with the value to destroy.
         */
        void Destroy(const std::string& hash,const std::string& key);


        /**
         * Number of key-value pairs contained in the unordered map with the given name.
         * @param  hash Name of the unordered map containing the key-value pairs.
         * @return      Number of key-value pairs contained in the unordered-map.
         */
        const int Length(const std::string& hash);

      private:

        /**
         * Map where all data is store.
         */
        std::shared_ptr<std::map<std::string,std::unordered_map<std::string,std::string>>> data_;


        /**
         * Mutex for multi-thread safety.
         */
        std::mutex mtx;
    };
  }
}
