#pragma once

// the linter wants this to be above everything else
#include <shared_mutex>

#include <atomic>
#include <memory>
#include <string>
#include <vector>

#include "all_type_variant.hpp"
#include "types.hpp"

namespace opossum {

class BaseIndex;
class BaseSegment;

// A chunk is a horizontal partition of a table.
// For each column in the table, it holds one segment. The segments across all chunks constitute the column.
//
// Find more information about this in our wiki: https://github.com/hyrise/hyrise/wiki/chunk-concept
class Chunk : private Noncopyable {
 public:
  Chunk() = default;

  // we need to explicitly set the move constructor to default when
  // we overwrite the copy constructor
  Chunk(Chunk&&) = default;
  Chunk& operator=(Chunk&&) = default;

  // adds a segment to the "right" of the chunk
  void add_segment(std::shared_ptr<BaseSegment> segment);

  // returns the number of columns (cannot exceed ColumnID (uint16_t))
  uint16_t column_count() const;

  // returns the number of rows (cannot exceed ChunkOffset (uint32_t))
  uint32_t size() const;

  // adds a new row, given as a list of values, to the chunk
  // note this is slow and not thread-safe and should be used for testing purposes only
  void append(const std::vector<AllTypeVariant>& values);

  // Returns the segment at a given position
  std::shared_ptr<BaseSegment> get_segment(ColumnID column_id) const;

 protected:
  std::vector<std::shared_ptr<BaseSegment>> segments;
};

}  // namespace opossum
