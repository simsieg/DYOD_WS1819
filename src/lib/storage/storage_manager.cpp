#include "storage_manager.hpp"

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "utils/assert.hpp"

namespace opossum {

std::shared_ptr<StorageManager> StorageManager::_instance = nullptr;

StorageManager& StorageManager::get() {
  static StorageManager _instance;
  return _instance;
}

void StorageManager::add_table(const std::string& name, std::shared_ptr<Table> table) { _tables[name] = table; }

void StorageManager::drop_table(const std::string& name) {
  auto result = _tables.erase(name);
  if (result == 0) {
    throw std::runtime_error("Error: Table not found!");
  }
}

std::shared_ptr<Table> StorageManager::get_table(const std::string& name) const { return _tables.at(name); }

bool StorageManager::has_table(const std::string& name) const { return _tables.count(name) != 0; }

std::vector<std::string> StorageManager::table_names() const {
  std::vector<std::string> keys;
  keys.reserve(_tables.size());
  for (auto entry : _tables) {
    keys.push_back(entry.first);
  }
  return keys;
}

void StorageManager::print(std::ostream& out) const {
  out << "-----------------------------------------------" << std::endl
      << "| Name | #Columns | #Rows | #Chunks |" << std::endl
      << "-----------------------------------------------" << std::endl;
  for (auto entry : _tables) {
    auto table = entry.second;
    out << "| " << entry.first << " | " << table->column_count() << " | " << table->row_count() << " | "
        << table->chunk_count() << " |" << std::endl;
  }
  out << "-----------------------------------------------" << std::endl;
}

void StorageManager::reset() { get() = StorageManager(); }
}  // namespace opossum
