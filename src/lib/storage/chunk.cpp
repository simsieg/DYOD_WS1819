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

void Chunk::add_segment(std::shared_ptr<BaseSegment> segment) {
  this->segments.push_back(segment);
}

void Chunk::append(const std::vector<AllTypeVariant>& values) {
  DebugAssert(values.size() == this->column_count(), "Error: Incorrect number of columns");
  
  for (int i = 0; i < this->column_count(); i++) {
    this->segments[i]->append(values[i]);
  }
}

std::shared_ptr<BaseSegment> Chunk::get_segment(ColumnID column_id) const {
  return this->segments[column_id];
}

uint16_t Chunk::column_count() const {
  return this->segments.size();
}

uint32_t Chunk::size() const {
  if (segments.size() > 0) {
    return this->segments[0]->size();
  } else {
    return 0;
  }
}

}  // namespace opossum
