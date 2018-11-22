#include <cmath>
#include <limits>

#include "fitted_attribute_vector.hpp"

#include "utils/assert.hpp"

namespace opossum {

template <typename uintX_t>
ValueID FittedAttributeVector<uintX_t>::get(const size_t element) const {
  DebugAssert(element >= 0 && element < _value_references.size(), "invalid value id (IndexOutOfBounds)");
  return ValueID(_value_references[element]);
}

template <typename uintX_t>
void FittedAttributeVector<uintX_t>::set(const size_t element, const ValueID value_id) {
  DebugAssert(value_id < std::pow(2, (8 * width())), "value id is too large (IndexOutOfBounds)");
  _value_references.insert(_value_references.begin() + element, value_id);
}

template <typename uintX_t>
size_t FittedAttributeVector<uintX_t>::size() const {
  return _value_references.size();
}

template <typename uintX_t>
AttributeVectorWidth FittedAttributeVector<uintX_t>::width() const {
  return AttributeVectorWidth(sizeof(uintX_t));
}

template <typename uintX_t>
void FittedAttributeVector<uintX_t>::reserve(const size_t expected_element_count) {
  _value_references.reserve(expected_element_count);
}

template <typename uintX_t>
void FittedAttributeVector<uintX_t>::append(const ValueID value_id) {
  _value_references.push_back(value_id);
}

}  // namespace opossum
