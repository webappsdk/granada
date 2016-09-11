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
  * Mage a cart using a shared map cache driver.
  *
  */

#pragma once
#include "cpprest/json.h"
#include "granada/cache/shared_map_cache_driver.h"
#include "granada/http/session/session.h"

namespace business{
  class Cart {
    public:

      /**
       * Constructor
       */
      Cart(granada::http::session::Session* session);


      /**
       * Returns number of products added to the cart.
       * @return Number of products.
       */
      int Count();


      /**
       * Add a product to the cart.
       * @param  product_id Product id.
       * @param  quantity   Quantity of the product we want to add.
       * @return            Number of products added to the cart.
       */
      int Add(const std::string product_id, int quantity);


      /**
       * Edit a product quantity.
       * @param  product_id Product id.
       * @param  quantity   New quantity of the product we want to overwrite.
       * @return            Number of products added to the cart.
       */
      int Edit(const std::string product_id, int quantity);


      /**
       * Removes a product from the cart.
       * @param  product_id Id of the product.
       * @return            Remaining number of products added to the cart.
       */
      int Remove(const std::string product_id);


      /**
       * Destroys the cart.
       */
      void Destroy();


      /**
       * Returns a stringified JSON array with a list of the
       * products added to the cart.
       * @return Stringified JSON array.
       */
      std::string List();


      /**
       * Change the cart owner, from anonymous to user.
       * Without a logged user, the cart is identified with the token of the session,
       * when logged, the cart is identified using the username.
       */
      void SwitchToUser();


    private:

      /**
       * Session containing the token used to identify the cart.
       */
      granada::http::session::Session* session_;


      /**
       * Cache where the carts are stored.
       */
      static std::unique_ptr<granada::cache::SharedMapCacheDriver> cache_;


      /**
       * Returns the name of the cart, based on the token of the session
       * or based on the username if the session has the USER role.
       * @return Name of the cart.
       */
      std::string GetHash();

  };
}
