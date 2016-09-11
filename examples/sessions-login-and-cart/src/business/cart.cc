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

#include "cart.h"

namespace business{

  std::unique_ptr<granada::cache::SharedMapCacheDriver> Cart::cache_ = std::unique_ptr<granada::cache::SharedMapCacheDriver>(new granada::cache::SharedMapCacheDriver());


  Cart::Cart(granada::http::session::Session* session){
    session_ = session;
  }


  int Cart::Count(){
    std::vector<std::string> keys;
    cache_->Match(GetHash() + "*",keys);
    return keys.size();
  }


  int Cart::Add(const std::string product_id, int quantity){
    std::string hash = GetHash();

    // first check if this product is not already added, if so sum quantities.
    std::string stored_quantity_str = cache_->Read(hash + product_id, "quantity");

    if (!stored_quantity_str.empty()){
      int stored_quantity = std::atoi(stored_quantity_str.c_str());
      quantity += stored_quantity;
    }

    cache_->Write(hash + product_id, "id", product_id);
    cache_->Write(hash + product_id, "quantity", std::to_string(quantity));

    return Count();
  }


  int Cart::Edit(const std::string product_id, int quantity){
    std::string hash = GetHash();

    if (quantity > 0){
      cache_->Write(hash + product_id, "quantity", std::to_string(quantity));
    }else{
      cache_->Destroy(hash + product_id);
    }
    return Count();
  }


  int Cart::Remove(const std::string product_id){
    cache_->Destroy(GetHash() + product_id);
    return 0;
  }


  void Cart::Destroy(){
    cache_->Destroy(GetHash() + "*");
  }


  std::string Cart::List(){
    std::vector<std::string> keys;
    cache_->Match(GetHash() + "*",keys);
    std::string list_str = "[";

    // loop throw all sessions.
    for ( auto it = keys.begin(); it != keys.end(); ++it ){
      if (it != keys.begin()){
        list_str += ",";
      }
      list_str += "{\"id\":\"" + cache_->Read(*it,"id") + "\",\"quantity\":\"" + cache_->Read(*it,"quantity") + "\"}";
    }

    list_str += "]";
    return list_str;
  }


  void Cart::SwitchToUser(){
    std::string old_hash = "cart:product:" + session_->GetToken() + ":";
    std::vector<std::string> keys;
    cache_->Match(old_hash + "*",keys);

    if (keys.size() > 0){
      std::string new_hash = GetHash();
      cache_->Destroy(new_hash + "*");
      std::string plugin_id;
      for ( auto it = keys.begin(); it != keys.end(); ++it ){
        plugin_id = cache_->Read(*it,"id");
        cache_->Write(new_hash + plugin_id,"id",plugin_id);
        cache_->Write(new_hash + plugin_id,"quantity",cache_->Read(*it,"quantity"));
      }
      cache_->Destroy(old_hash + "*");
    }
  }


  std::string Cart::GetHash(){
    if (session_->roles()->Is("USER")){
      return "cart:product:" + session_->roles()->GetProperty("USER", "username") + ":";
    }else{
      return "cart:product:"+session_->GetToken() + ":";
    }
  }
}
