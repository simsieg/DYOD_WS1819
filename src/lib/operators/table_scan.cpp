#include "table_scan.hpp"

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "../storage/table.hpp"
#include "abstract_operator.hpp"
#include "all_type_variant.hpp"
#include "types.hpp"
#include "utils/assert.hpp"

namespace opossum {
TableScan::TableScan(const std::shared_ptr<const AbstractOperator> in, ColumnID column_id, const ScanType scan_type,
                     const AllTypeVariant search_value)
    : _in{in}, _column_id{column_id}, _scan_type{scan_type}, _search_value{search_value} {}

ColumnID TableScan::column_id() const { return _column_id; }
ScanType TableScan::scan_type() const { return _scan_type; }
const AllTypeVariant& TableScan::search_value() const { return _search_value; }

std::shared_ptr<const Table> TableScan::_on_execute() {
  const auto result_table = std::make_shared<const Table>();
  // TODO(all): implement table scan
  return result_table;
}

}  // namespace opossum
