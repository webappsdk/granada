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
  */

#include "granada/functions.h"

namespace granada{

  FunctionsMapIterator::FunctionsMapIterator(granada::FunctionsMap* functions_map){
    mtx_ = functions_map->GetMtx();
    functions_ = functions_map->GetAll();
    reset();
  };

  void FunctionsMapIterator::reset(){
    mtx_->lock();
    it_ = functions_->begin();
    mtx_->unlock();
  };

  bool FunctionsMapIterator::has_next(){
    mtx_->lock();
    if (it_ != functions_->end()){
      mtx_->unlock();
      return true;
    }
    mtx_->unlock();
    return false;
  };

  Function FunctionsMapIterator::next(){
    mtx_->lock();
    Function function;
    function.name = it_->first;
    function.function = it_->second;
    it_++;
    mtx_->unlock();
    return function;
  };


  bool FunctionsMap::Has(const std::string& name){
    if (!name.empty()){
      mtx_.lock();
      auto it = functions_->find(name);
      if (it!=functions_->end()){
        mtx_.unlock();
        return true;
      }
      mtx_.unlock();
    }
    return false;
  };


  void FunctionsMap::Add(const std::string& name, function_json_json fn){
    std::cout << "[++] Add function : " << name << "\n\n";
    if (!name.empty()){
      mtx_.lock();
      (*functions_)[name] = fn;
      mtx_.unlock();
    }
  };
  

  void FunctionsMap::Remove(const std::string& name){
    mtx_.lock();
    auto it = functions_->find(name);
    if (it!=functions_->end()){
      functions_->erase(it);
    }
    mtx_.unlock();
  };


  function_json_json FunctionsMap::Get(const std::string& name){
    mtx_.lock();
    function_json_json fn = (*this)[name];
    mtx_.unlock();
    return fn;
  }


  function_json_json FunctionsMap::operator [](const std::string& name){
    mtx_.lock();
    auto it = functions_->find(name);
    if (it!=functions_->end()){
      function_json_json fn = it->second;
      mtx_.unlock();
      return fn;
    }
    mtx_.unlock();
    return &Functions::UndefinedErrorFunction;
  }


  void FunctionsMap::Call(const std::string& name, web::json::value& parameters, function_void_json callback){
    web::json::value response = (*this)[name](parameters);
    callback(response);
  };
  

  void FunctionsMap::Call(const std::string& name, web::json::value& parameters){
    (*this)[name](parameters);
  };


  void FunctionsMap::Call(const std::string& name, function_void_json callback){
    web::json::value parameters = Functions::DefaultParameters();
    Call(name,parameters,callback);
  };


  void FunctionsMap::Call(const std::string& name){
    web::json::value parameters = Functions::DefaultParameters();
    Call(name,parameters);
  };


  std::shared_ptr<FunctionsIterator> FunctionsMap::make_iterator(){
    return std::shared_ptr<FunctionsIterator>(new granada::FunctionsMapIterator(this));
  };


  void FunctionsMap::CallAll(web::json::value& parameters,function_void_json callback){
    web::json::value response = web::json::value::object();
    web::json::value data = web::json::value::object();
    mtx_.lock();
    for (auto it = functions_->begin(); it != functions_->end(); ++it){
      web::json::value partial_response = it->second(parameters);
      web::json::value partial_response_wrapper = web::json::value::object();
      partial_response_wrapper["data"] = partial_response;
      data[it->first] = partial_response_wrapper;
    }
    mtx_.unlock();
    response["data"] = data;
    callback(response);
  };
  

  void FunctionsMap::CallAll(function_void_json callback){
    web::json::value parameters = Functions::DefaultParameters();
    CallAll(parameters,callback);
  };


  void FunctionsMap::CallAll(web::json::value& parameters){
    mtx_.lock();
    for (auto it = functions_->begin(); it != functions_->end(); ++it){
      it->second(parameters);
    }
    mtx_.unlock();
  };


  void FunctionsMap::CallAll(){
    web::json::value parameters;
    CallAll(parameters);
  };

}
