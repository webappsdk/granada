#include "granada/crypto/nonce_generator.h"

namespace granada{
  namespace crypto{
    std::unique_ptr<utility::nonce_generator> CPPRESTNonceGenerator::n_generator_ = std::unique_ptr<utility::nonce_generator>(new utility::nonce_generator());
  }
}
