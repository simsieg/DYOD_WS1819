#pragma once

#include <vector>

#include "base_attribute_vector.hpp"
#include "types.hpp"

namespace opossum {

// FittedAttributeVector, implementation of BaseAttributeVector
template <typename uintX_t>
class FittedAttributeVector : public BaseAttributeVector {
 public:
  FittedAttributeVector() = default;
  ~FittedAttributeVector() = default;

  // returns the value at a given positon
  ValueID get(const size_t i) const override;

  // sets the value_id at a given position
  void set(const size_t i, const ValueID value_id) override;

  // returns the number of values
  size_t size() const override;

  // returns the width of the values in bytes
  AttributeVectorWidth width() const override;

 protected:
  std::vector<uintX_t> _value_references;
};
}  // namespace opossum
