/**
  * Copyright (c) <2016> Web App SDK granada <afernandez@cookinapps.io>
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
  * Functions: Manages a function collection in memory, can add and remove functions 
  * identified with a string name and call them by their name or call them all
  * at once.
  * Iterator: Iterates over the functions of a collection of functions.
  *
  */

#pragma once
#include <string>
#include <map>
#include "cpprest/json.h"

namespace granada{

  /**
   * Function type with JSON as a parameter and that returns a JSON.
   */
  typedef std::function<web::json::value(const web::json::value&)> function_json_json;


  /**
   * Function type with JSON as a parameter.
   */
  typedef std::function<void(const web::json::value&)> function_void_json;


  /**
   * Pair of function name and function.
   * The functions accept a json value as parameter and
   * return a json value.
   */
  struct Function{
    std::string name;
    function_json_json function;
  };


  class Functions;

  /**
   * Interface for functions iterators.
   * Iterates over the functions of a collection of functions.
   */
  class FunctionsIterator{

    public:
      
      /**
       * Constructor
       */
      FunctionsIterator(){};


      /**
       * Constructor
       */
      FunctionsIterator(granada::Functions* functions){};


      /**
       * Destructor
       */
      virtual ~FunctionsIterator(){};


      /**
       * Reset the iterator to the begin.
       */
      virtual void reset(){};


      /**
       * Returns true if there is a next function after the actual
       * in the collection. False if there is not.
       * @return  True if there is a next function after the actual
       *          in the collection. False if there is not.
       *                   
       */
      virtual bool has_next(){ return false; };


      /**
       * Returns the function in the following position.
       * Call has_next() first to check if it exists.
       */
      virtual Function next(){ return Function(); };

  };


  /**
   * Interface for managing functions collections.
   * Manages a function collection in memory, can add and remove functions 
   * identified with a string name and call them by their name or call them all
   * at once. The functions accept a json value as parameter and
   * return a json value.
   */
  class Functions{

    public:

      /**
       * Destructor
       */
      virtual ~Functions(){};


      /**
       * Return true if function with given name is in the collection
       * and false if it is not.
       * 
       * @param name  Name of the function.
       * @return      True if function with given name is in the collection
       *              and false if it is not.
       */
      virtual bool Has(const std::string& name){ return false; };


      /**
       * Adds a function to the collection.
       * 
       * @param name  Name of the function.
       * @param fn    Function.
       */
      virtual void Add(const std::string& name, function_json_json fn){};


      /**
       * Removes function with given name from the collection.
       * 
       * @param name Name of the function.
       */
      virtual void Remove(const std::string& name){};


      /**
       * Returns a function with given name.
       * 
       * @param name  Name of the function.
       * @return      Function with JSON as a parameter and that returns a JSON.
       */
      virtual function_json_json Get(const std::string& name){ return &Functions::UndefinedErrorFunction; };


      /**
       * Returns a function with given name.
       * 
       * @param name  Name of the function.
       * @return      Function with JSON as a parameter and that returns a JSON.
       */
      virtual function_json_json operator [](const std::string& name){ return &Functions::UndefinedErrorFunction; };


      /**
       * Calls a function by its name passing a json value as parameters
       * to the called function, accepts a callback to get the value
       * returned by the called function.
       * 
       * @param name        Name of the function to call.
       * @param parameters  Parameters in form of json value to pass to the
       *                    called function.
       * @param callback    Callback function to get the value returned by
       *                    the called function. Function with JSON as a parameter.
       */
      virtual void Call(const std::string& name, const web::json::value& parameters, function_void_json callback){};


      /**
       * Calls a function by its name passing a json value as parameters
       * to the called function.
       * 
       * @param name        Name of the function to call.
       * @param parameters  Parameters in form of json value to pass to the
       *                    called function.
       */
      virtual void Call(const std::string& name, const web::json::value& parameters){};


      /**
       * Calls a function by its name, accepts a callback to get the
       * value returned by the called function.
       * 
       * @param name        Name of the function to call.
       * @param callback    Callback function to get the value returned by
       *                    the called function. Function with JSON as a parameter.
       */
      virtual void Call(const std::string& name, function_void_json callback){};


      /**
       * Calls a function by its name.
       * 
       * @param name        Name of the function to call.
       */
      virtual void Call(const std::string& name){};


      /**
       * Calls all functions of the collection passing a json value as parameters
       * to the called functions, accepts a callback to get the values
       * returned by the called functions. The values returned by the
       * functions will be structured in a json object and passed through the
       * callback parameters like this: {
       *                                    "data":
       *                                            {
       *                                              "RemoveUserEntries" : {
       *                                                 "data" : {"success":true}
       *                                              },
       *                                              "RemoveUserClipboard" : {
       *                                                 "data" : {"success":true}
       *                                              },
       *                                            }
       *                                 }
       * 
       * @param name        Name of the function to call.
       * @param parameters  Parameters in form of json value to pass to the
       *                    called function.
       * @param callback    Callback function to get the value returned by
       *                    the called function. Function with JSON as a parameter.
       */
      virtual void CallAll(const web::json::value& parameters,function_void_json callback){};


      /**
       * Calls all functions of the collection accepts a callback to get the values
       * returned by the called functions. The values returned by the
       * functions will be structured in a json object and passed through the
       * callback parameters like this: {
       *                                    "data":
       *                                            {
       *                                              "RemoveUserEntries" : {
       *                                                 "data" : {"success":true}
       *                                              },
       *                                              "RemoveUserClipboard" : {
       *                                                 "data" : {"success":true}
       *                                              },
       *                                            }
       *                                 }
       * 
       * @param callback    Callback function to get the value returned by
       *                    the called function. Function with JSON as a parameter.
       */
      virtual void CallAll(function_void_json callback){};


      /**
       * Calls all functions of the collection passing a json value as parameters
       * to the called functions.
       * 
       * @param parameters  JSON value containing the parameters to pass to the
       *                    called functions.
       */
      virtual void CallAll(const web::json::value& parameters){};


      /**
       * Calls all functions of the collection.
       */
      virtual void CallAll(){};


      /**
       * Returns an iterator to iterate over the functions of the collection.
       * @return    Iterator to iterate over the functions of the collection.
       */
      virtual std::shared_ptr<FunctionsIterator> make_iterator(){ return std::shared_ptr<FunctionsIterator>(new FunctionsIterator()); };


    protected:

      /**
       * Function used to tell the user the function he/she is trying to call
       * does not exist.
       */
      static web::json::value UndefinedErrorFunction(const web::json::value& data){
        web::json::value response = web::json::value::object();
        response["error"] = web::json::value::string("undefined_function");
      };


      /**
       * JSON used as the called function default parameters in case the
       * user does not specify them.
       */
      static inline web::json::value DefaultParameters(){
        return web::json::value::object();
      }
  };

  class FunctionsMap;

  class FunctionsMapIterator : public FunctionsIterator{

    public:

      FunctionsMapIterator(granada::FunctionsMap* functions_map);

      /**
       * Reset the iterator.
       */
      virtual void reset();


      /**
       * Returns true if iterator is not equal to end.
       * @return  True if the iterator is not equal to end, false if it is.
       */
      virtual bool has_next();


      /**
       * Returns the next function in the collection.
       */
      virtual Function next();


    private:

      /**
       * Map iterator
       */
      std::map<std::string,function_json_json>::iterator it_;
      

      /**
       * Pointer to map containing the functions and their identifying name.
       */
      std::shared_ptr<std::map<std::string,function_json_json>> functions_;


      std::mutex* mtx_;
  };


  /**
   * Functions collection, stored using a map as container.
   */
  class FunctionsMap : public Functions{

    public:

      /**
       * Constructor
       */        
      FunctionsMap(){
        functions_ = std::shared_ptr<std::map<std::string,function_json_json>>(new std::map<std::string,function_json_json>());
      };


      /**
       * Destructor
       */
      virtual ~FunctionsMap(){};


      /**
       * Return true if function with given name is in the collection
       * and false if it is not.
       * 
       * @param name  Name of the function.
       * @return      True if function with given name is in the collection
       *              and false if it is not.
       */
      virtual bool Has(const std::string& name);


      /**
       * Adds a function to the collection of functions, and identify it with
       * a name so it can be called or removed by the given name.
       * The name of a function is unique, if you add two functions with the
       * same name, the last one will be the one present in the collection.
       * @param   name  Name of the function
       * @param   fn    Function.
       */
      virtual void Add(const std::string& name, function_json_json fn);
      

      /**
       * Removes function from the collection with given name.
       * @param   name  Name of the function.
       */
      virtual void Remove(const std::string& name);


      /**
       * Returns the function from the collection identified with given name.
       * If function does not exist in the collection a function returning an
       * JSON object : {"error":"undefined_function"} is returned.
       * @param   name      Name of the function
       * @return  function  Function.
       */
      virtual function_json_json Get(const std::string& name);


      /**
       * Returns the function from the collection identified with given name.
       * If function does not exist in the collection a function returning an
       * JSON object : {"error":"undefined_function"} is returned.
       * @param   name      Name of the function
       * @return  function  Function.
       */
      virtual function_json_json operator [](const std::string& name);


      /**
       * Calls a function from the collection with the given name identifier and with given parameters.
       * After the function is called a callback function will be called with the return of the called function.
       * @param   name        Name identifying the function.
       * @param   parameters  JSON to pass to the function to call.
       * @param   callback    Callback function.
       */
      virtual void Call(const std::string& name, const web::json::value& parameters, function_void_json callback);
      

      /**
       * Calls a function from the collection with the given name identifier and with given parameters.
       * @param   name        Name identifying the function.
       * @param   parameters  JSON to pass to the function to call.
       */
      virtual void Call(const std::string& name, const web::json::value& parameters);


      /**
       * Calls a function from the collection with the given name identifier.
       * After the function is called a callback function will be called with the return of the called function.
       * @param   name        Name identifying the function.
       * @param   callback    Callback function.
       */
      virtual void Call(const std::string& name, function_void_json callback);


      /**
       * Calls a function from the collection with the given name identifier.
       * @param   name        Name identifying the function.
       */
      virtual void Call(const std::string& name);


      /**
       * Calls all functions from the collection with given parameters.
       * After the function is called a callback function will be called with the returns of the called functions
       * contained in a json object like: {
       *                                    "data":
       *                                            {
       *                                              "RemoveUserEntries" : {
       *                                                 "data" : {"success":true}
       *                                              },
       *                                              "RemoveUserClipboard" : {
       *                                                 "data" : {"success":true}
       *                                              },
       *                                            }
       *                                 }
       * @param   parameters  JSON to pass to the functions.
       * @param   callback    Callback function with a JSON array containing the returns of the functions.
       */
      virtual void CallAll(const web::json::value& parameters,function_void_json callback);
      

      /**
       * Calls all functions from the collection.
       * After the function is called a callback function will be called with the returns of the called functions
       * contained in a json object like: {
       *                                    "data":
       *                                            {
       *                                              "RemoveUserEntries" : {
       *                                                 "data" : {"success":true}
       *                                              },
       *                                              "RemoveUserClipboard" : {
       *                                                 "data" : {"success":true}
       *                                              },
       *                                            }
       *                                 }
       * @param   callback    Callback function with a JSON array containing the returns of the functions.
       */
      virtual void CallAll(function_void_json callback);


      /**
       * Calls all functions from the collection with given parameters.
       * @param   parameters  JSON to pass to the functions.
       */
      virtual void CallAll(const web::json::value& parameters);


      /**
       * Calls all functions from the collection.
       */
      virtual void CallAll();

      
      /**
       * Returns a pointer to the map containing all the functions.
       */
      virtual std::shared_ptr<std::map<std::string,function_json_json>>& GetAll(){
        return functions_;
      };


      /**
       * Returns a pointer to the mutex.
       */
      virtual std::mutex* GetMtx(){
        return &mtx_;
      };


      /**
       * Returns a function iterator to iterate over all the functions of the collection.
       */
      virtual std::shared_ptr<FunctionsIterator> make_iterator();


    protected:

      /**
       * Mutex for preventing multithread problems
       * when accessign to the functions.
       */
      std::mutex mtx_;

      /**
       * Map containing the functions and its identifying name.
       */
      std::shared_ptr<std::map<std::string,function_json_json>> functions_;
  };
}
