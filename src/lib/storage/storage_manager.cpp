#include "storage_manager.hpp"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "utils/assert.hpp"

namespace opossum {

StorageManager& StorageManager::get() {
  return *(new StorageManager());
  // A really hacky fix to get the tests to run - replace this with your implementation
}

void StorageManager::add_table(const std::string& name, std::shared_ptr<Table> table) {
  // Implementation goes here
}

void StorageManager::drop_table(const std::string& name) {
  // Implementation goes here
}

std::shared_ptr<Table> StorageManager::get_table(const std::string& name) const {
  // Implementation goes here
  return nullptr;
}

bool StorageManager::has_table(const std::string& name) const {
  // Implementation goes here
  return false;
}

std::vector<std::string> StorageManager::table_names() const {
  throw std::runtime_error("Implement StorageManager::table_names");
}

void StorageManager::print(std::ostream& out) const {
  // Implementation goes here
}

void StorageManager::reset() {
  // Implementation goes here;
}

}  // namespace opossum
