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
  * Cipher and decipher
  *
  */

#pragma once

namespace granada{
  namespace crypto{
    class Cryptograph{
      public:

        /**
         * Constructor
         */
        Cryptograph(){};


        /**
         * Destructor
         */
        virtual ~Cryptograph(){};


        /**
         * Encrypt a text with a password.
         * @param  text     Text to encrypt.
         * @param  password Password to use to encrypt text.
         * @return          Encrypted text.
         */
        virtual std::string Encrypt(const std::string& text, std::string password){ return std::string(); };


        /**
         * Decrypt a text with given password.
         * @param  crypted_text Text to decrypt.
         * @param  password     Password to use to decrypt text.
         * @return              Decrypted text.
         */
        virtual std::string Decrypt(const std::string& text, std::string password){ return std::string(); };
    };
  }
}
