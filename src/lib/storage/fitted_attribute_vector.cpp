#include "fitted_attribute_vector.hpp"

#include <limits>

namespace opossum {

template <typename uintX_t>
ValueID FittedAttributeVector<uintX_t>::get(const ChunkOffset i) const {
  return ValueID(_value_references.at(i));
}

template <typename uintX_t>
void FittedAttributeVector<uintX_t>::set(const ChunkOffset i, const ValueID value_id) {
  // TODO: Check and optimize
  _value_references[i] = value_id;
}

template <typename uintX_t>
size_t FittedAttributeVector<uintX_t>::size() const {
  return _value_references.size();
}

template <typename uintX_t>
AttributeVectorWidth FittedAttributeVector<uintX_t>::width() const {
  return AttributeVectorWidth(sizeof(uintX_t));
}

template class FittedAttributeVector<uint8_t>;
template class FittedAttributeVector<uint16_t>;
template class FittedAttributeVector<uint32_t>;
}  // namespace opossum