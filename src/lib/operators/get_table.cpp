#include "get_table.hpp"

#include <memory>
#include <string>

#include "storage/storage_manager.hpp"

namespace opossum {
GetTable::GetTable(const std::string& name) { _table_name = name; }

const std::string& GetTable::table_name() const { return _table_name; }

std::shared_ptr<const Table> GetTable::_on_execute() { return StorageManager::get().get_table(_table_name); }

}  // namespace opossum
