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
  * Classes to manage cache, data stored as key-value pairs, data can be persistant
  * or not.
  *
  */
#pragma once
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace granada{
  namespace cache{

    /**
     * Interface. Iterates over cache keys.
     */
    class CacheHandlerIterator{

      public:


        /**
         * Constructor
         */
        CacheHandlerIterator(){};


        /**
         * Constructor
         * @param   expression  Expression to search keys.
         *                      
         *                      Example:
         *                      
         *                          session:value:*
         *                          => can be used to search all the keys
         *                          starting with "session:value"
         *                          
         *                          *:value:*
         *                          => can be used to  search all keys
         *                          containing ":value:"
         */
        CacheHandlerIterator(const std::string& expression){
          set(expression);
        };


        /**
         * Destructor
         */
        virtual ~CacheHandlerIterator(){};


        /**
         * Set the iterator, useful to reuse it.
         * @param expression Filter pattern/expression.
         *                   Example:
         *                   			session:*TOKEN46464* => will SCAN or KEYS keys that match the given expression.
         */
        virtual void set(const std::string& expression){
          expression_.assign(expression);
        };


        /**
         * Return true if there is another value with same pattern, false
         * if thereis not.
         * @return True | False
         */
        virtual const bool has_next(){ return false; };


        /**
         * Return the next key found with the given pattern.
         * @return [description]
         */
        virtual const std::string next(){ return std::string(); };


      protected:

        /**
         * Filter pattern/expression.
         * SCAN or KEYS keys that match the given expression.
         * 
         * Example:
         *                      
         *                          session:value:*
         *                          => can be used to search all the keys
         *                          starting with "session:value"
         *                          
         *                          *:value:*
         *                          => can be used to  search all keys
         *                          containing ":value:"
         */
        std::string expression_;

    };


    /**
     * Manages cache, data stored as key-value pairs, data can be persistant
     * or not.
     */
    class CacheHandler
    {

      public:


        /**
         * Contructor
         */
        CacheHandler(){};


        /**
         * Destructor
         */
        virtual ~CacheHandler(){};


        /**
         * Returns true if key exists, false if it does not.
         * @param  key Key.
         * @return     True if key exists, false if it does not.
         */
        virtual const bool Exists(const std::string& key){ return false; };


        /**
         * Checks if a key exist in a set with given hash.
         * @param  hash Name of the set of key-value.
         * @param  key  Key of the value
         * @return      True if exist, false if it does not.
         */
        virtual const bool Exists(const std::string& hash,const std::string& key){ return false; };


        /**
         * Returns value from the cache.
         * @param  key Key of the value.
         * @return     Value
         */
        virtual const std::string Read(const std::string& key){ return std::string(); };


        /**
         * Returns the value stored in a set and associated with the given key.
         * @param  hash Name of the set where the key-value pairs are stored.
         * @param  key  Key associated with the value.
         * @return      Value.
         */
        virtual const std::string Read(const std::string& hash, const std::string& key){ return std::string(); };


        /**
         * Fills a vector of strings with the the keys that match an expression.
         * 
         * @param expression  Expression.
         *                      Example:
         *                          *:GHs98Ev4GLkqw32g8
         *                          
         * @param keys        Vector of strings that should be filled with the keys matching
         *                    the given expression/pattern.
         *                      Example:
         *                          plugin:value:GHs98Ev4GLkqw32g8
         *                          plugin.handler:value:GHs98Ev4GLkqw32g8
         *                          plugin:store:GHs98Ev4GLkqw32g8
         */
        virtual const void Match(const std::string& expression, std::vector<std::string>& keys){
          keys.clear();
          std::shared_ptr<granada::cache::CacheHandlerIterator> cache_iterator = this->make_iterator(expression);
          while (cache_iterator->has_next()){
            keys.push_back(cache_iterator->next());
          }
        };


        /**
         * Set a value in the cache associated with a given key.
         * @param key   Key of the value.
         * @param value Value.
         */
        virtual void Write(const std::string& key,const std::string& value){};


        /**
         * Inserts or rewrite a key-value pair in a set with the given name.
         * If the set does not exist, it creates it.
         * @param hash  Name of the set.
         * @param key   Key to identify the value inside the set.
         * @param value Value
         */
        virtual void Write(const std::string& hash,const std::string& key,const std::string& value){};


        /**
         * Removes a key-value pair from the cache.
         * @param key
         */
        virtual void Destroy(const std::string& key){};


        /**
         * Destroy a key-value pair stored in a set.
         * @param hash Name of the set where the key-value pair is stored.
         * @param key  Key associated with the value.
         */
        virtual void Destroy(const std::string& hash,const std::string& key){};


        /**
         * Returns an iterator to iterate over keys with an expression.
         */
        virtual std::shared_ptr<granada::cache::CacheHandlerIterator> make_iterator(const std::string& expression){
          return std::make_shared<granada::cache::CacheHandlerIterator>(expression);
        };
        
    };
  }
}
