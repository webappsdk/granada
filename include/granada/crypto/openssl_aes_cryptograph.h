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
  * Encrypt and decrypt using openssl AES
  *
  */

#pragma once
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cryptograph.h"
#include <openssl/aes.h>

namespace granada{
  namespace crypto{
    class OpensslAESCryptograph : public Cryptograph{
      public:

        /**
         * Constructor
         */
        OpensslAESCryptograph(){};


        // override
        std::string Encrypt(const std::string& text, std::string password){

          password = password + "0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz";

          unsigned char text_to_encrypt[text.size()+1];
          memcpy(text_to_encrypt,text.c_str(),text.size()+1);

          unsigned char key[password.size()+1];
          memcpy(key,password.c_str(),password.size()+1);

          unsigned char enc_out[AES_SIZE];

          AES_KEY enc_key;

          AES_set_encrypt_key(key, AES_SIZE, &enc_key);
          AES_encrypt(text_to_encrypt, enc_out, &enc_key);

          return std::string(reinterpret_cast<char*>(enc_out));
        };

        // override
        std::string Decrypt(const std::string& text, std::string password){
          password = password + "0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz";
          unsigned char key[password.size()+1];
          memcpy(key,password.c_str(),password.size()+1);
          unsigned char dec_out[AES_SIZE];
          AES_KEY dec_key;
          AES_set_decrypt_key(key,AES_SIZE,&dec_key);
          unsigned char crypted_text[text.size()+1];
          memcpy(crypted_text,text.c_str(),text.size()+1);
          AES_decrypt(crypted_text, dec_out, &dec_key);

          return std::string(reinterpret_cast<char*>(dec_out));
        };

      private:
        const int AES_SIZE = 128;

    };
  }
}
