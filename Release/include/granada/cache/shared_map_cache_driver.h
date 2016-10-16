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
  * unordered map
  * 	|_ hash1 => map
  * 								|_ key1 => value1
  * 							 	|_ key2 => value2
  * 	|_ hash2 => map
  * 								|_ key1 => value3
  * 							 	|_ key2 => value4
  *
  * This code is multi-thread safe.
  *
  */

#pragma once
#include "cache_handler.h"
#include <regex>
#include <string>
#include <deque>
#include <unordered_map>
#include <map>
#include "granada/util/string.h"

namespace granada{
  namespace cache{


    class SharedMapCacheDriver;

    /**
     * Tool for iterate over cache keys with a given pattern.
     */
    class SharedMapIterator : public CacheHandlerIterator{

      public:

        /**
         * Constructor
         */
        SharedMapIterator(){};


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
        SharedMapIterator(const std::string& expression, SharedMapCacheDriver* cache);


        /**
         * Destructor
         */
        virtual ~SharedMapIterator(){};


        /**
         * Set the iterator, useful to reuse it.
         * @param expression Filter pattern/expression.
         *                   Example:
         *                              session:*TOKEN46464* => will SCAN or KEYS keys that match the given expression.
         */
        virtual void set(const std::string& expression) override;


        /**
         * Return true if there is another value with same pattern, false
         * if there is not.
         * @return True | False
         */
        virtual const bool has_next() override;


        /**
         * Return the next key found with the given pattern.
         * @return [description]
         */
        virtual const std::string next() override;


      protected:

        /**
         * Cache containing the keys to iterate.
         */
        SharedMapCacheDriver* cache_;


        /**
         * Vector for storing found keys.
         */
        std::vector<std::string> keys_;


        /**
         * Iterator.
         */
        std::vector<std::string>::iterator it_;

    };


    /**
     * Manages the cache storing key-value pairs in an unordered map.
     * unordered map
     *   |_ hash1 => map
     *                 |_ key1 => value1
     *                 |_ key2 => value2
     *   |_ hash2 => map
     *                 |_ key1 => value3
     *                 |_ key2 => value4
     *
     * This code is multi-thread safe.
     */
    class SharedMapCacheDriver : public CacheHandler
    {
      public:

        /**
         * Constructor
         */
        SharedMapCacheDriver();


        /**
         * Destructor
         */
        virtual ~SharedMapCacheDriver(){};


        /**
         * Checks if a key exist in the cache.
         * @param  key  Key to check.
         */
        virtual const bool Exists(const std::string& key) override;


        /**
         * Checks if a key exist in a set with given hash.
         * @param  hash Name of the set of key-value.
         * @param  key  Key of the value
         * @return      True if exist, false if it does not.
         */
        virtual const bool Exists(const std::string& hash,const std::string& key) override;


        /**
         * Returns value from the cache.
         * @param  key Key of the value.
         * @return     Value
         */
        virtual const std::string Read(const std::string& key) override;


        /**
         * Returns the value of a key-value pair stored in
         * an map with the given name.
         * @param  hash Name of the map.
         * @param  key  Key to identify the value.
         * @return      Value.
         */
        virtual const std::string Read(const std::string& hash,const std::string& key) override;


        /**
         * Set a value in the cache associated with a given key.
         * @param key   Key of the value.
         * @param value Value.
         */
        virtual void Write(const std::string& key,const std::string& value) override;


        /**
         * Inserts or rewrite a key-value pair in a map with the given name.
         * If the set does not exist, it creates it.
         * @param  hash Name of the map.
         * @param  key  Key to identify the value.
         * @param       Value.
         */
        virtual void Write(const std::string& hash,const std::string& key,const std::string& value) override;


        /**
         * Destroys a set of key-value pairs with the given name.
         * @param hash Name of the unordered map containing the key-value pairs
         */
        virtual void Destroy(const std::string& key) override;


        /**
         * Destroy a key value pair of a given set.
         * @param hash Name of the unordered map containing the key-value pair to destroy.
         * @param key  Key associated with the value to destroy.
         */
        virtual void Destroy(const std::string& hash,const std::string& key) override;


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
        virtual std::unique_ptr<granada::cache::CacheHandlerIterator> make_iterator(const std::string& expression) override{
          return granada::util::memory::make_unique<granada::cache::SharedMapIterator>(expression,this);
        };


      protected:

        /**
         * Map where all data is stored.
         */
        std::unique_ptr<std::unordered_map<std::string,std::map<std::string,std::string>>> data_;

        /**
         * Mutex for thread safety.
         */
        std::mutex mtx;


    };
  }
}
