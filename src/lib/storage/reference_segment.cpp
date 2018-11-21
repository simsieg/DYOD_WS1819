#include "reference_segment.hpp"

#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "base_segment.hpp"
#include "dictionary_segment.hpp"
#include "table.hpp"
#include "value_segment.hpp"

namespace opossum {

// ReferenceSegment is a specific segment type that stores all its values as position list of a referenced segment

ReferenceSegment::ReferenceSegment(const std::shared_ptr<const Table> referenced_table, const ColumnID referenced_column_id,
    const std::shared_ptr<const PosList> pos):
    _referenced_table{referenced_table},
    _referenced_column_id{referenced_column_id},
    _pos{pos} {

    }
const AllTypeVariant ReferenceSegment::operator[](const size_t i) const {
    DebugAssert(i < _referenced_table->row_count(), "Index out of bounds");
    RowID row{_pos->operator[](i)};
    return _referenced_table->get_chunk(row.chunk_id).get_segment(_referenced_column_id)->operator[](row.chunk_offset);
}
size_t ReferenceSegment::size() const {
    return _pos->size();
}
const std::shared_ptr<const PosList> ReferenceSegment::pos_list() const {
    return _pos;
}
const std::shared_ptr<const Table> ReferenceSegment::referenced_table() const {
    return _referenced_table;
}
ColumnID ReferenceSegment::referenced_column_id() const {
    return _referenced_column_id;
}

}  // namespace opossum
