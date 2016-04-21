/**
  * Copyright (c) <2016> HTML Puzzle Team <htmlpuzzleteam@gmail.com>
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

#include "cart.h"

namespace business{

  std::unique_ptr<granada::cache::SharedMapCacheDriver> Cart::cache_ = std::unique_ptr<granada::cache::SharedMapCacheDriver>(new granada::cache::SharedMapCacheDriver());


  Cart::Cart(granada::http::session::Session* session){
    session_ = session;
  }


  int Cart::Count(){
    return cache_->Length(GetHash());
  }


  int Cart::Add(const std::string product_id, int quantity){
    std::string hash = GetHash();

    // first check if this product is not already added, if so sum quantities.
    std::string stored_quantity_str = cache_->Read(hash,product_id);

    if (!stored_quantity_str.empty()){
      int stored_quantity = std::atoi(stored_quantity_str.c_str());
      quantity += stored_quantity;
    }

    cache_->Write(hash,product_id,std::to_string(quantity));

    return Count();
  }


  int Cart::Edit(const std::string product_id, int quantity){
    std::string hash = GetHash();

    if (quantity > 0){
      cache_->Write(hash,product_id,std::to_string(quantity));
    }else{
      cache_->Destroy(hash,product_id);
    }
    return Count();
  }


  int Cart::Remove(const std::string product_id){
    cache_->Destroy(GetHash(),product_id);
    return 0;
  }


  void Cart::Destroy(){
    cache_->Destroy(GetHash());
  }


  std::string Cart::List(){
    std::unordered_map<std::string,std::string> products = cache_->GetProperties(GetHash());
    std::string list_str = "[";

    // loop throw all sessions.
    for ( auto it = products.begin(); it != products.end(); ++it ){
      if (it != products.begin()){
        list_str += ",";
      }
      list_str += "{\"id\":\"" + it->first + "\",\"quantity\":\"" + it->second + "\"}";
    }

    list_str += "]";
    return list_str;
  }


  void Cart::SwitchToUser(){
    std::string old_hash = "cart:" + session_->GetToken();
    if (cache_->Length(old_hash) > 0){
      std::unordered_map<std::string,std::string> products = cache_->GetProperties(old_hash);
      cache_->Write(GetHash(),products);
    }
    cache_->Destroy(old_hash);
  }


  std::string Cart::GetHash(){
    if (session_->roles()->Is("USER")){
      return "cart:" + session_->roles()->GetProperty("USER", "username");
    }else{
      return "cart:"+session_->GetToken();
    }
  }
}
