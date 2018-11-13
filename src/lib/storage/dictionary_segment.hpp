#pragma once

#include <algorithm>
#include <limits>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "all_type_variant.hpp"
#include "fitted_attribute_vector.hpp"
#include "type_cast.hpp"
#include "types.hpp"
#include "value_segment.hpp"

namespace opossum {

class BaseAttributeVector;
class BaseSegment;

// Even though ValueIDs do not have to use the full width of ValueID (uint32_t), this will also work for smaller ValueID
// types (uint8_t, uint16_t) since after a down-cast INVALID_VALUE_ID will look like their numeric_limit::max()
constexpr ValueID INVALID_VALUE_ID{std::numeric_limits<ValueID::base_type>::max()};

// Dictionary is a specific segment type that stores all its values in a vector
template <typename T>
class DictionarySegment : public BaseSegment {
 public:
  /**
   * Creates a Dictionary segment from a given value segment.
   */
  explicit DictionarySegment(const std::shared_ptr<BaseSegment>& value_base_segment) {
    const auto value_segment = std::static_pointer_cast<ValueSegment<T>>(value_base_segment);
    const auto rows = value_segment->size();

    _dictionary_vector = _create_dictionary(value_segment->values());
    _attribute_vector = _create_fittedattributevector(rows);

    for (const auto& value : value_segment->values()) {
      const auto search_iter = std::find(_dictionary_vector->cbegin(), _dictionary_vector->cend(), value);
      _attribute_vector->append(ValueID(
              std::distance(_dictionary_vector->cbegin(), search_iter)));
    }
  }
  // Since most of these methods depend on the template parameter, they need to be implemented in this file

  // return the value at a certain position. If you want to write efficient operators, back off!
  const AllTypeVariant operator[](const size_t i) const override {
    const ValueID id = ValueID(_attribute_vector->get(i));
    return _dictionary_vector->at(id);
  }

  // return the value at a certain position.
  const T get(const size_t i) const {
    const ValueID id = ValueID(_attribute_vector->get(i));
    return _dictionary_vector->at(id);
  }

  // dictionary segments are immutable
  void append(const AllTypeVariant&) override { throw std::runtime_error("Can not append to full chunk"); }

  // returns an underlying dictionary
  std::shared_ptr<const std::vector<T>> dictionary() const { return _dictionary_vector; }

  // returns an underlying data structure
  std::shared_ptr<const BaseAttributeVector> attribute_vector() const { return _attribute_vector; }

  // return the value represented by a given ValueID
  const T& value_by_value_id(ValueID value_id) const { return _dictionary_vector->at(value_id); }

  // returns the first value ID that refers to a value >= the search value
  // returns INVALID_VALUE_ID if all values are smaller than the search value
  ValueID lower_bound(T value) const {
    const auto search_iter = std::lower_bound(_dictionary_vector->cbegin(), _dictionary_vector->cend(), value);
    if (search_iter == _dictionary_vector->cend()) {
      return INVALID_VALUE_ID;
    }
    return ValueID(search_iter - _dictionary_vector->cbegin());
  }

  // same as lower_bound(T), but accepts an AllTypeVariant
  ValueID lower_bound(const AllTypeVariant& value) const { return lower_bound(type_cast<T>(value)); }

  // returns the first value ID that refers to a value > the search value
  // returns INVALID_VALUE_ID if all values are smaller than or equal to the search value
  ValueID upper_bound(T value) const {
    const auto search_iter = std::upper_bound(_dictionary_vector->cbegin(), _dictionary_vector->cend(), value);
    if (search_iter == _dictionary_vector->cend()) {
      return INVALID_VALUE_ID;
    }
    return ValueID(search_iter - _dictionary_vector->cbegin());
  }

  // same as upper_bound(T), but accepts an AllTypeVariant
  ValueID upper_bound(const AllTypeVariant& value) const { return upper_bound(type_cast<T>(value)); }

  // return the number of unique_values (dictionary entries)
  size_t unique_values_count() const { return _dictionary_vector->size(); }

  // return the number of entries
  size_t size() const override { return _attribute_vector->size(); }

 protected:
  std::shared_ptr<std::vector<T>> _dictionary_vector;
  std::shared_ptr<BaseAttributeVector> _attribute_vector;

  std::shared_ptr<std::vector<T>> _create_dictionary(const std::vector<T>& segment_values) const {
    auto values_list = std::make_shared<std::vector<T>>(std::move(segment_values));

    std::sort(values_list->begin(), values_list->end());
    auto uniqueness_end_iter = std::unique(values_list->begin(), values_list->end());
    values_list->erase(uniqueness_end_iter, values_list->cend());

    return values_list;
  }

  std::shared_ptr<BaseAttributeVector> _create_fittedattributevector(size_t row_count) const {
    std::shared_ptr<BaseAttributeVector> return_vector = nullptr;
    if (row_count <= UINT8_MAX) {
      return_vector = std::make_shared<FittedAttributeVector<uint8_t>>();
    } else if (row_count <= UINT16_MAX) {
      return_vector = std::make_shared<FittedAttributeVector<uint16_t>>();
    } else if (row_count <= UINT32_MAX) {
      return_vector = std::make_shared<FittedAttributeVector<uint32_t>>();
    } else {
      return_vector = std::make_shared<FittedAttributeVector<uint64_t>>();
    }
    return_vector->reserve(row_count);
    return return_vector;
  }
};

}  // namespace opossum
