#pragma once

#include <memory>
#include <string>
#include "cpprest/asyncrt_utils.h"

namespace granada{
  namespace crypto{
    class NonceGenerator{
      public:
        NonceGenerator(){};
        
        virtual std::string generate(int& length){
          return std::string();
        };
    };

    class CPPRESTNonceGenerator : public NonceGenerator{


      public:
        CPPRESTNonceGenerator(){};

        std::string generate(int& length){
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
