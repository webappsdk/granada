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
  * Manages the cache storing key-value pairs or sets of key-value pairs using redis
  * data structure server (http://redis.io/).
  * It uses redisclient by Alex Nekipelov https://github.com/nekipelov/redisclient
  * This code is multi-thread safe.
  *
  */
#pragma once

#include <string>
#include "granada/defaults.h"
#include "granada/util/mutex.h"
#include "granada/util/application.h"
#include "cache_handler.h"
#include "redisclient/redissyncclient.h"



namespace granada{
  namespace cache{

    class RedisCacheDriver;

    /**
     * Redis Sync client wrapper to ensure multithread safety by
     * having only one Redis client per application.
     */
    class RedisSyncClientWrapper{

      public:

        /**
         * Constructor.
         */
        RedisSyncClientWrapper();


        /**
         * Returns redis sync client pointer.
         * @return Redis sync client Pointer.
         */
        redisclient::RedisSyncClient* get(){
          return redis_.get();
        };


      private:

        /**
         * Used for loading the properties only once.
         */
        static granada::util::mutex::call_once load_properties_call_once_;


        /**
         * Redis sync client.
         */
        std::unique_ptr<redisclient::RedisSyncClient> redis_;


        /**
         * Loaded in LoadProperties() function, will take the value
         * of the "redis_cache_driver_address" property. If the property
         * is not provided default_strings::redis_cache_redis_address will be taken instead.
         */
        static std::string redis_address_;


        /**
         * Loaded in LoadProperties() function, will take the value
         * of the "redis_cache_driver_port" property. If the property
         * is not provided default_strings::redis_cache_redis_port will be taken instead.
         */
        static unsigned short redis_port_;


        /**
         * Load properties for configuring the redis server connection.
         */
        void LoadProperties();


        /**
         * Init Redis Sync Client.
         * @param redis    Redis Sync client pointer.
         * @param _address Redis server adress.
         * @param port     Redis server port.
         */
        void ConnectRedisSyncClient(redisclient::RedisSyncClient* redis, const std::string& _address, const unsigned short& port);
    };



    /**
     * Iterates over cache keys.
     * Tool for SCAN or KEYS search in a Redis database, with a given pattern.
     */
    class RedisIterator : public CacheHandlerIterator{

      public:

        /**
         * Type of search, SCAN or KEYS
         */
        enum Type {KEYS = 0, SCAN = 1};


        /**
         * Constructor.
         */
        RedisIterator(){};


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
         */
        RedisIterator(const std::string& expression);


        /**
         * Constructor
         * 
         * @param type          Type of search KEYS or SCAN
         * @param expression    Filter pattern/expression.
         *                      Example:
         *                              session:*TOKEN46464* => will SCAN or KEYS keys that match the given expression.
         */
        RedisIterator(RedisIterator::Type type, const std::string& expression);


        /**
         * Destructor
         */
        virtual ~RedisIterator(){};


        /**
         * Set the iterator, useful to reuse it.
         * @param expression Filter pattern/expression.
         *                   Example:
         *                              session:*TOKEN46464* => will SCAN or KEYS keys that match the given expression.
         */
        virtual void set(const std::string& expression) override;


        /**
         * Set the iterator, useful to reuse it.
         * @param type       SCAN or KEYS command.
         * @param expression Filter pattern/expression.
         *                   Example:
         *                   			session:*TOKEN46464* => will SCAN or KEYS keys that match the given expression.
         */
        void set(RedisIterator::Type type, const std::string& expression);


        /**
         * Returns true if there is another value with same pattern, false
         * if there is not.
         * @return True | False
         */
        virtual const bool has_next();


        /**
         * Returns the next key found with the given pattern.
         * @return next key in the keys_ vector.
         */
        virtual const std::string next();


      protected:

        /**
         * Manager of the storage, and contains
         * the data stored.
         */
        static std::unique_ptr<granada::cache::RedisCacheDriver> cache_;


        /**
         * Results of the SCAN or KEYS search.
         */
        std::vector<redisclient::RedisValue> keys_;


        /**
         * Index where we are in the results array.
         */
        int index_ = 0;


        /**
         * If SCAN search the cursor of the SCAN set we are in.
         */
        std::string cursor_ = "";


        /**
         * Search type: SCAN or KEYS
         */
        Type type_;


        /**
         * True if there is a value to be returned.
         */
        bool has_next_ = false;


        /**
         * Get the next vector with data of SCAN or KEYS
         */
        void GetNextVector();
    };


    /**
     * Manages the cache storing key-value pairs or sets of key-value pairs using redis
     * data structure server (http://redis.io/).
     * It uses redisclient by Alex Nekipelov https://github.com/nekipelov/redisclient
     * This code is multi-thread safe.
     */
    class RedisCacheDriver : public CacheHandler
    {
      public:

        /**
         * Controler
         */
        RedisCacheDriver(){};


        /**
         * Destructor
         */
        virtual ~RedisCacheDriver(){};


        /**
         * Checks if a key exist in the cache.
         * @param  key  Key to check.
         */
        virtual const bool Exists(const std::string& key);


        /**
         * Checks if a key exist in a set with given hash.
         * @param  hash Name of the set of key-value.
         * @param  key  Key of the value
         * @return      True if exist, false if it does not.
         */
        virtual const bool Exists(const std::string& hash,const std::string& key);


        /**
         * Returns the value associated with the given key.
         * @param  key Key associated with the value.
         * @return     Value.
         */
        virtual const std::string Read(const std::string& key);


        /**
         * Returns the value stored in a set and associated with the given key.
         * @param  hash Name of the set where the key-value pairs are stored.
         * @param  key  Key associated with the value.
         * @return      Value.
         */
        virtual const std::string Read(const std::string& hash, const std::string& key);


        /**
         * Inserts a key-value pair, rewrites it if it already exists.
         * @param key   Key to identify the value.
         * @param value Value
         */
        virtual void Write(const std::string& key,const std::string& value);


        /**
         * Inserts or rewrite a key-value pair in a set with the given name.
         * If the set does not exist, it creates it.
         * @param hash  Name of the set.
         * @param key   Key to identify the value inside the set.
         * @param value Value
         */
        virtual void Write(const std::string& hash,const std::string& key,const std::string& value);


        /**
         * Destroys a key-value pair or a set of values.
         * @param key Key of the value or name of the set to destroy.
         */
        virtual void Destroy(const std::string& key);


        /**
         * Destroys a key-value pair stored in a set.
         * @param hash Name of the set where the key-value pair is stored.
         * @param key  Key associated with the value.
         */
        virtual void Destroy(const std::string& hash,const std::string& key);


        /**
         * Renames a key if it does not already exists.
         * 
         * @param old_key Old key to rename.
         * @param new_key New key.
         * 
         * @return        True if the key could be renamed, false if not, for
         *                example it will return false if the new key already existed.
         */
        virtual bool Rename(const std::string& old_key, const std::string& new_key);


        /**
         * Returns a RedisValue containing a group of keys of the
         * cache that match a given expression for a given cursor,
         * returns also a new cursor to obtain a new group of keys.
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
         * @return            RedisValue containing a group keys and a new cursor.
         */
        redisclient::RedisValue Scan(const std::string& cursor, const std::string& expression_);


        /**
         * Returns a RedisValue containing all the keys of the
         * cache that match a given expression.
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
         * @return            RedisValue containing all the keys.
         */
        redisclient::RedisValue Keys(const std::string& expression_);


        /**
         * Returns an iterator to iterate over keys with an expression.
         * @param   Expression to be use to iterate over keys that match this expression.
         *          Example: "user*" => we will iterate over all the keys that start with "user"
         * @return  Iterator.
         */
        virtual std::unique_ptr<granada::cache::CacheHandlerIterator> make_iterator(const std::string& expression){
          return granada::util::memory::make_unique<granada::cache::RedisIterator>(expression);
        };


      protected:

        /**
         * Redis client.
         */
        static std::unique_ptr<RedisSyncClientWrapper> redis_;


        /**
         * Mutex for thread safety.
         */
        static std::mutex mtx_;


    };
  }
}
