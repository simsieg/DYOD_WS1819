#pragma once

#include <memory>
#include <string>

#include "all_type_variant.hpp"
#include "types.hpp"

namespace opossum {

// BaseSegment is the abstract super class for all segment types,
// e.g., ValueSegment, ReferenceSegment
class BaseSegment : private Noncopyable {
 public:
  BaseSegment() = default;
  virtual ~BaseSegment() = default;

  // we need to explicitly set the move constructor to default when
  // we overwrite the copy constructor
  BaseSegment(BaseSegment&&) = default;
  BaseSegment& operator=(BaseSegment&&) = default;

  // returns the value at a given position
  virtual const AllTypeVariant operator[](const size_t i) const = 0;

  // appends the value at the end of the segment
  virtual void append(const AllTypeVariant& val) = 0;

  // returns the number of values
  virtual size_t size() const = 0;
};
}  // namespace opossum
