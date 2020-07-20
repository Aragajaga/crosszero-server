#ifndef CROSSZERO_UTIL_CRYPTO_HPP_
#define CROSSZERO_UTIL_CRYPTO_HPP_

#include <openssl/sha.h>
#include "types.hpp"

namespace util {

void sha256_hash(byte_t* dest, const byte_t* data, const std::size_t size) {
  // TODO: Do checks
  SHA256_CTX context;
  SHA256_Init(&context);
  SHA256_Update(&context, reinterpret_cast<const byte_t*>(data), size);
  SHA256_Final(dest, &context);
}

}  // namespace util

#endif  // CROSSZERO_UTIL_CRYPTO_HPP_
