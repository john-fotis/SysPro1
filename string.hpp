#ifndef STRING_HPP
#define STRING_HPP

#include <sstream>

template <typename T>
inline std::string toString(const T &value) {
  std::stringstream ss;
  ss << value;
  return ss.str();
}

inline unsigned int myStoi(std::string str) {
  unsigned i = 0;
  unsigned int result = 0;
  while (str[i] >= '0' && str[i] <= '9') {
    result = result * 10 + (str[i] - '0');
    i++;
  }
  return result;
}

#endif