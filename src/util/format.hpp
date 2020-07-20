#ifndef CROSSZERO_UTIL_FORMAT_HPP_
#define CROSSZERO_UTIL_FORMAT_HPP_

#include <string>
#include <sstream>
#include <iomanip>

#include "types.hpp"

namespace util {

std::string bin2hex(const byte_t* data, const std::size_t length) {
  std::stringstream ss;
  for (std::size_t i; i < length; i++) {
    ss << std::hex << std::setfill('0') << std::setw(2) << data[i];
  }
  return ss.str();
}

}  // namespace util

#endif  // CROSSZERO_UTIL_FORMAT_HPP_
