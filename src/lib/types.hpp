#pragma once

#include <cstdint>
#include <iostream>
#include <limits>
#include <string>
#include <tuple>
#include <vector>

#include "strong_typedef.hpp"

/**
 * We use STRONG_TYPEDEF to avoid things like adding chunk ids and value ids.
 * Because implicit constructors are deleted, you cannot initialize a ChunkID
 * like this
 *   ChunkID x = 3;
 * but need to use
 *   ChunkID x{3}; or
 *   auto x = ChunkID{3};
 *
 * WorkerID, TaskID, CommitID, and TransactionID are used in std::atomics and
 * therefore need to be trivially copyable. That's currently not possible with
 * the strong typedef (as far as I know).
 *
 * TODO(anyone): Also, strongly typing ChunkOffset causes a lot of errors in
 * the group key and adaptive radix tree implementations. Unfortunately, I
 * wasn’t able to properly resolve these issues because I am not familiar with
 * the code there
 */

STRONG_TYPEDEF(uint32_t, ChunkID);
STRONG_TYPEDEF(uint16_t, ColumnID);
STRONG_TYPEDEF(uint32_t, ValueID);  // Cannot be larger than ChunkOffset

namespace opossum {

using ChunkOffset = uint32_t;
using AttributeVectorWidth = uint8_t;

struct RowID {
  ChunkID chunk_id;
  ChunkOffset chunk_offset;

  // Joins need to use RowIDs as keys for maps.
  bool operator<(const RowID& rhs) const {
    return std::tie(chunk_id, chunk_offset) < std::tie(rhs.chunk_id, rhs.chunk_offset);
  }

  bool operator==(const RowID& rhs) const {
    return std::tie(chunk_id, chunk_offset) == std::tie(rhs.chunk_id, rhs.chunk_offset);
  }
};

enum class ScanType { OpEquals, OpNotEquals, OpLessThan, OpLessThanEquals, OpGreaterThan, OpGreaterThanEquals };

using PosList = std::vector<RowID>;

class Noncopyable {
 protected:
  Noncopyable() = default;
  Noncopyable(Noncopyable&&) = default;
  Noncopyable& operator=(Noncopyable&&) = default;
  ~Noncopyable() = default;
  Noncopyable(const Noncopyable&) = delete;
  const Noncopyable& operator=(const Noncopyable&) = delete;
};

}  // namespace opossum
