#include "storage_manager.hpp"

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "utils/assert.hpp"

namespace opossum {

StorageManager& StorageManager::get() {
  static StorageManager singleton_instance;
  return singleton_instance;
}

void StorageManager::add_table(const std::string& name, std::shared_ptr<Table> table) { _tables[name] = table; }

void StorageManager::drop_table(const std::string& name) {
  auto result = _tables.erase(name);
  DebugAssert(result == 1, "Table '" + name + "' not found");
}

std::shared_ptr<Table> StorageManager::get_table(const std::string& name) const { return _tables.at(name); }

bool StorageManager::has_table(const std::string& name) const { return _tables.count(name) != 0; }

std::vector<std::string> StorageManager::table_names() const {
  std::vector<std::string> names;

  names.reserve(_tables.size());

  std::transform(_tables.cbegin(), _tables.cend(), std::back_inserter(names),
          [](auto const& tables_map_entry){ return tables_map_entry.first; });

  return names;
}

void StorageManager::print(std::ostream& out) const {
  // header
  out << "-----------------------------------------------" << std::endl
      << "| Name | #Columns | #Rows | #Chunks |" << std::endl
      << "-----------------------------------------------" << std::endl;

  // content
  std::for_each( _tables.cbegin(), _tables.cend(),
          [&out](auto const& tables_map_entry) {
            auto table = tables_map_entry.second;
            out << "| " << tables_map_entry.first << " | " << table->column_count() << " | "
            << table->row_count() << " | " << table->chunk_count() << " |" << std::endl;
  });

  // footer
  out << "-----------------------------------------------" << std::endl;
}

void StorageManager::reset() { get() = StorageManager(); }
}  // namespace opossum
