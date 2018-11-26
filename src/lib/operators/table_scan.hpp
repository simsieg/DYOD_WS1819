#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "../storage/dictionary_segment.hpp"
#include "../storage/reference_segment.hpp"
#include "../storage/table.hpp"
#include "../storage/value_segment.hpp"
#include "abstract_operator.hpp"
#include "all_type_variant.hpp"
#include "types.hpp"
#include "utils/assert.hpp"

namespace opossum {

class BaseTableScanImpl {
 public:
  virtual std::shared_ptr<const Table> scan() = 0;
};

class Table;

class TableScan : public AbstractOperator {
 public:
  TableScan(const std::shared_ptr<const AbstractOperator> in, ColumnID column_id, const ScanType scan_type,
            const AllTypeVariant search_value);

  ~TableScan() = default;

  ColumnID column_id() const;
  ScanType scan_type() const;
  const AllTypeVariant& search_value() const;

 protected:
  std::shared_ptr<const Table> _on_execute() override;
  ColumnID _column_id;
  const ScanType _scan_type;
  const AllTypeVariant _search_value;

  template <typename T>
  class TableScanImpl : public BaseTableScanImpl {
   public:
    TableScanImpl(const std::shared_ptr<const Table> table, ColumnID column_id, const ScanType scan_type,
                  const AllTypeVariant search_value);

    std::shared_ptr<const Table> scan() override;

   protected:
    const std::shared_ptr<const Table> _table;
    const ColumnID _column_id;
    const ScanType _scan_type;
    const T _search_value;
    void _scan_reference_segment(const std::shared_ptr<ReferenceSegment> segment, const T search_value,
                                 const std::shared_ptr<PosList> pos_list);
    void _scan_value_segment(const std::shared_ptr<ValueSegment<T>> segment, const T search_value,
                             const std::shared_ptr<PosList> pos_list);
    void _scan_dictionary_segment(const std::shared_ptr<DictionarySegment<T>> segment, const T search_value,
                                  const std::shared_ptr<PosList> pos_list);
  };
};

}  // namespace opossum
