#include <iomanip>
#include <iterator>
#include <limits>
#include <memory>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

#include "base_segment.hpp"
#include "chunk.hpp"

#include "utils/assert.hpp"

namespace opossum {

void Chunk::add_segment(std::shared_ptr<BaseSegment> segment) { _segments.push_back(segment); }

void Chunk::append(const std::vector<AllTypeVariant>& values) {
  DebugAssert(values.size() == column_count(), "Incorrect number of columns");

  auto value_iter = values.cbegin();
  std::for_each(_segments.begin(), _segments.end(),
                [&value_iter](const auto& segment) { segment->append(*value_iter++); });
}

std::shared_ptr<BaseSegment> Chunk::get_segment(ColumnID column_id) const { return _segments[column_id]; }

uint16_t Chunk::column_count() const { return static_cast<uint16_t>(_segments.size()); }

uint32_t Chunk::size() const {
  if (column_count() > 0) {
    return static_cast<uint32_t>(_segments[0]->size());
  }
  return 0;
}

bool Chunk::compression_started() const { return _compression_started; }

void Chunk::set_compression_start() { _compression_started = true; }

}  // namespace opossum
