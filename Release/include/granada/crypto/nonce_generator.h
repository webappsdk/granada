#pragma once

#include <memory>
#include <string>
#include "cpprest/asyncrt_utils.h"

namespace granada{
  namespace crypto{
    class NonceGenerator{
      public:

        /**
         * Constructor
         */
        NonceGenerator(){};


        /**
         * Destructor
         */
        virtual ~NonceGenerator(){};


        /**
         * Generate a nonce with the given length.
         * 
         * @param length  Length of the nonce.
         * @return        Nonce.
         */
        virtual std::string generate(int& length){
          return std::string();
        };
    };

    class CPPRESTNonceGenerator : public NonceGenerator{


      public:

        /**
         * Constructor
         */
        CPPRESTNonceGenerator(){};


        /**
         * Destructor
         */
        virtual ~CPPRESTNonceGenerator(){};


        /**
         * Generate a random alphanumeric string
         * with the given length.
         * 
         * @param length  Length of the nonce.
         * @return        Nonce.
         */
        std::string generate(int& length) override {
          n_generator_->set_length(length);
          return n_generator_->generate();
        };

      private:
        
        /**
         * Nonce string generator, for generating unique strings tokens.
         * From cpprest/asyncrt_utils.h
         * Generate a nonce string containing random alphanumeric characters (A-Za-z0-9).
         */
        static std::unique_ptr<utility::nonce_generator> n_generator_;
    };
  }
}
