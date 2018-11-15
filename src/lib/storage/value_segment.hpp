#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "base_segment.hpp"

namespace opossum {

// ValueSegment is a segment type that stores all its values in a vector
template <typename T>
class ValueSegment : public BaseSegment {
 public:
  // return the value at a certain position. If you want to write efficient operators, back off!
  const AllTypeVariant operator[](const size_t offset) const override;

  // add a value to the end
  void append(const AllTypeVariant& val) override;

  // return the number of entries
  size_t size() const override;

  // Return all values. This is the preferred method to check a value at a certain index. Usually you need to
  // access more than a single value anyway.
  // e.g. const auto& values = value_segment.values(); and then: values[i]; in your loop.
  const std::vector<T>& values() const;

 protected:
  // Implementation goes here
};

}  // namespace opossum
