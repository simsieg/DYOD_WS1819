#include "table_scan.hpp"

#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "../storage/dictionary_segment.hpp"
#include "../storage/reference_segment.hpp"
#include "../storage/table.hpp"
#include "../storage/value_segment.hpp"
#include "abstract_operator.hpp"
#include "all_type_variant.hpp"
#include "resolve_type.hpp"
#include "types.hpp"
#include "utils/assert.hpp"

namespace opossum {

TableScan::TableScan(const std::shared_ptr<const AbstractOperator> in, ColumnID column_id, const ScanType scan_type,
                     const AllTypeVariant search_value)
    : AbstractOperator(in), _column_id{column_id}, _scan_type{scan_type}, _search_value{search_value} {}

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
    : _table{table}, _column_id{column_id}, _scan_type{scan_type}, _search_value{type_cast<T>(search_value)} {
  // TODO(all): Check type of column and search value
}

template <typename T>
void TableScan::TableScanImpl<T>::_scan_value_segment(const std::shared_ptr<ValueSegment<T>> segment,
                                                      const std::shared_ptr<PosList> pos_list, const ChunkID chunk_id) {
  const std::vector<T>& values = segment->values();
  for (ChunkOffset index = 0; index < segment->size(); ++index) {
    if (_dispatched_comparator(_scan_type, values.at(index), _search_value)) {
      pos_list->emplace_back(RowID{chunk_id, index});
    }
  }
}

template <typename T>
void TableScan::TableScanImpl<T>::_scan_dictionary_segment(const std::shared_ptr<DictionarySegment<T>> segment,
                                                           const std::shared_ptr<PosList> pos_list,
                                                           const ChunkID chunk_id) {
  const auto& attribute_vector = segment->attribute_vector();
  for (ChunkOffset index = 0; index < attribute_vector->size(); ++index) {
    if (_dispatched_comparator(_scan_type, segment->value_by_value_id(attribute_vector->get(index)), _search_value)) {
      pos_list->emplace_back(RowID{chunk_id, index});
    }
  }
}

template <typename T>
void TableScan::TableScanImpl<T>::_scan_reference_segment(const std::shared_ptr<ReferenceSegment> segment,
                                                          const std::shared_ptr<PosList> pos_list,
                                                          const ChunkID chunk_id) {
  // TODO(all): Scan for search value and add positions to position list
}

template <typename T>
std::shared_ptr<const Table> TableScan::TableScanImpl<T>::scan() {
  // Prepare result table
  const auto result_table = std::make_shared<Table>();
  for (auto column_id = ColumnID{0}; column_id < _table->column_count(); column_id++) {
    result_table->add_column_definition(_table->column_name(column_id), _table->column_type(column_id));
  }

  // Determine segment type and scan for search value
  for (auto chunk_id = ChunkID{0}; chunk_id < _table->chunk_count(); chunk_id++) {
    // Initialize chunk position list
    const auto chunk_pos_list = std::make_shared<PosList>();
    const auto segment = _table->get_chunk(chunk_id).get_segment(_column_id);

    const auto reference_segment = std::dynamic_pointer_cast<ReferenceSegment>(segment);
    if (reference_segment != nullptr) {
      _scan_reference_segment(reference_segment, chunk_pos_list, chunk_id);
      Chunk chunk;
      for (auto column_id = ColumnID{0}; column_id < result_table->column_count(); column_id++) {
        // Create a reference segment for every segment with the current chunk pos list
        chunk.add_segment(
            std::make_shared<ReferenceSegment>(reference_segment->referenced_table(), column_id, chunk_pos_list));
      }
      result_table->emplace_chunk(std::move(chunk));
      continue;
    }

    const auto dictionary_segment = std::dynamic_pointer_cast<DictionarySegment<T>>(segment);
    if (dictionary_segment != nullptr) {
      _scan_dictionary_segment(dictionary_segment, chunk_pos_list, chunk_id);
      Chunk chunk;
      for (auto column_id = ColumnID{0}; column_id < result_table->column_count(); column_id++) {
        // Create a reference segment for every segment with the current chunk pos list
        chunk.add_segment(
            std::make_shared<ReferenceSegment>(reference_segment->referenced_table(), column_id, chunk_pos_list));
      }
      result_table->emplace_chunk(std::move(chunk));
      continue;
    }

    const auto value_segment = std::dynamic_pointer_cast<ValueSegment<T>>(segment);
    if (value_segment != nullptr) {
      _scan_value_segment(value_segment, chunk_pos_list, chunk_id);
      Chunk chunk;
      for (auto column_id = ColumnID{0}; column_id < result_table->column_count(); column_id++) {
        // Create a reference segment for every segment with the current chunk pos list
        chunk.add_segment(
            std::make_shared<ReferenceSegment>(reference_segment->referenced_table(), column_id, chunk_pos_list));
      }
      result_table->emplace_chunk(std::move(chunk));
      continue;
    }

    throw std::runtime_error("Error: Can not scan unknown segment type");
  }

  return result_table;
}

template <typename T>
bool _dispatched_comparator(ScanType scan_type, T& value, T& search_value) {
  switch (scan_type) {
    case ScanType::OpEquals: {
      return value == search_value;
      break;
    }
    case ScanType::OpNotEquals: {
      return value != search_value;
      break;
    }
    case ScanType::OpGreaterThan: {
      return value > search_value;
      break;
    }
    case ScanType::OpGreaterThanEquals: {
      return value >= search_value;
      break;
    }
    case ScanType::OpLessThan: {
      return value < search_value;
      break;
    }
    case ScanType::OpLessThanEquals: {
      return value <= search_value;
      break;
    }
    default: { return false; };
  }
}

}  // namespace opossum
