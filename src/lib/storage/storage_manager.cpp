#include "storage_manager.hpp"

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "utils/assert.hpp"

namespace opossum {

std::shared_ptr<StorageManager> StorageManager::instance = NULL;

StorageManager& StorageManager::get() {
  static StorageManager instance;
  return instance;
}

void StorageManager::add_table(const std::string& name, std::shared_ptr<Table> table) { this->tables[name] = table; }

void StorageManager::drop_table(const std::string& name) {
  auto result = this->tables.erase(name);
  if (result == 0) {
    throw std::runtime_error("Error: Table not found!");
  }
}

std::shared_ptr<Table> StorageManager::get_table(const std::string& name) const { return this->tables.at(name); }

bool StorageManager::has_table(const std::string& name) const { return this->tables.count(name) != 0; }

std::vector<std::string> StorageManager::table_names() const {
  std::vector<std::string> keys;
  for (auto entry : this->tables) {
    keys.reserve(this->tables.size());
    keys.push_back(entry.first);
  }
  return keys;
}

void StorageManager::print(std::ostream& out) const {
  out << "-----------------------------------------------" << std::endl
      << "| Name | #Columns | #Rows | #Chunks |" << std::endl
      << "-----------------------------------------------" << std::endl;
  for (auto entry : this->tables) {
    auto table = entry.second;
    out << "| " << entry.first << " | " << table->column_count() << " | " << table->row_count() << " | "
        << table->chunk_count() << " |" << std::endl;
  }
  out << "-----------------------------------------------" << std::endl;
}

void StorageManager::reset() { get() = StorageManager(); }
}  // namespace opossum
