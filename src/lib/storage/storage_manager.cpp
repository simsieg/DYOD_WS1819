#include "storage_manager.hpp"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "utils/assert.hpp"

namespace opossum {

std::shared_ptr<StorageManager> StorageManager::instance = NULL;

StorageManager& StorageManager::get() {
  static StorageManager instance;
  return instance;
}

void StorageManager::add_table(const std::string& name, std::shared_ptr<Table> table) {
  this->_table_names.push_back(name);
  this->_table_contents.push_back(table);
}

void StorageManager::drop_table(const std::string& name) {
  for (uint16_t i = 0; i < this->_table_names.size(); i++) {
    if (this->_table_names[i].compare(name) == 0) {
      this->_table_names.erase(this->_table_names.begin() + i);
      this->_table_contents.erase(this->_table_contents.begin() + i);
      return;
    }
  }
  throw std::runtime_error("Error: Table does not exist!");
}

std::shared_ptr<Table> StorageManager::get_table(const std::string& name) const {
  for (uint16_t i = 0; i < this->_table_names.size(); i++) {
    if (this->_table_names[i].compare(name) == 0) {
      return this->_table_contents[i];
    }
  }
  throw std::runtime_error("Error: Table does not exist!");
}

bool StorageManager::has_table(const std::string& name) const {
  for (uint16_t i = 0; i < this->_table_names.size(); i++) {
    if (this->_table_names[i].compare(name) == 0) {
      return true;
    }
  }
  return false;
}

std::vector<std::string> StorageManager::table_names() const { return this->_table_names; }

void StorageManager::print(std::ostream& out) const {
  out << "-----------------------------------------------" << std::endl
      << "| Name | #Columns | #Rows | #Chunks |" << std::endl
      << "-----------------------------------------------" << std::endl;
  for (uint16_t i = 0; i < this->_table_names.size(); i++) {
    auto table = this->_table_contents[i];
    out << "| " << _table_names[i] << " | " << table->column_count() << " | " << table->row_count() << " | "
        << table->chunk_count() << " |" << std::endl;
  }
  out << "-----------------------------------------------" << std::endl;
}

void StorageManager::reset() { get() = StorageManager(); }
}  // namespace opossum
