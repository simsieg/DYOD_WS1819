#pragma once

#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace opossum {

class Table;

template <typename T>
std::vector<T> _split(const std::string& str, char delimiter) {
  std::vector<T> internal;
  std::stringstream ss(str);
  std::string tok;

  while (std::getline(ss, tok, delimiter)) {
    internal.push_back(tok);
  }

  return internal;
}

// This is a helper method which is heavily used in our test suite
std::shared_ptr<Table> load_table(const std::string& file_name, size_t chunk_size);

}  // namespace opossum
