#include "table_scan.hpp"

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "../storage/table.hpp"
#include "abstract_operator.hpp"
#include "all_type_variant.hpp"
#include "resolve_type.hpp"
#include "types.hpp"
#include "utils/assert.hpp"

namespace opossum {

TableScan::TableScan(const std::shared_ptr<const AbstractOperator> in, ColumnID column_id, const ScanType scan_type,
                     const AllTypeVariant search_value)
    : _column_id{column_id}, _scan_type{scan_type}, _search_value{search_value} {}

ColumnID TableScan::column_id() const { return _column_id; }
ScanType TableScan::scan_type() const { return _scan_type; }
const AllTypeVariant& TableScan::search_value() const { return _search_value; }

std::shared_ptr<const Table> TableScan::_on_execute() {
  const auto input_table = _input_table_left();
  const auto table_scan_impl = make_unique_by_data_type<BaseTableScanImpl, TableScanImpl>(
      input_table->column_type(_column_id), input_table, _column_id, _scan_type, _search_value);
  return table_scan_impl->scan();
}

template <typename T>
TableScan::TableScanImpl<T>::TableScanImpl(const std::shared_ptr<const Table> table, ColumnID column_id,
                                           const ScanType scan_type, const AllTypeVariant search_value)
    : _table{table}, _column_id{column_id}, _scan_type{scan_type}, _search_value{type_cast<T>(search_value)} {}

template <typename T>
std::shared_ptr<const Table> TableScan::TableScanImpl<T>::scan() {
  const auto result = std::make_shared<Table>();

  // TODO(all): Implement scan algorithm

  return result;
}

}  // namespace opossum
