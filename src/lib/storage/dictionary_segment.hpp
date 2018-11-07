#pragma once

#include <limits>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "value_segment.hpp"

#include "all_type_variant.hpp"
#include "types.hpp"
#include "type_cast.hpp"

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
  explicit DictionarySegment(const std::shared_ptr<BaseSegment>& base_segment) {
    // Cast to ValueSegment
    const auto value_segment = std::static_pointer_cast<ValueSegment<T>>(base_segment);
    _dictionary = std::make_shared<std::vector<T>>(std::move(value_segment->values()));    
    
    // Init dictionary
    std::sort(_dictionary->begin(), _dictionary->end());
    const auto vector_iter = std::unique(_dictionary->begin(), _dictionary->end());
    _dictionary->erase(vector_iter, _dictionary->end());
    
    // Init attribute vector
    _attribute_vector = std::make_shared<std::vector<uint32_t>>();
    for (const auto value : value_segment->values()) {
      const auto index = std::find(_dictionary->begin(), _dictionary->end(), value);
      _attribute_vector->push_back(index - _dictionary->begin());
    }
  };
  // SEMINAR INFORMATION: Since most of these methods depend on the template parameter, you will have to implement
  // the DictionarySegment in this file. Replace the method signatures with actual implementations.

  // return the value at a certain position. If you want to write efficient operators, back off!
  const AllTypeVariant operator[](const size_t i) const override {
    const ValueID id = ValueID(_attribute_vector->at(i));
    return _dictionary->at(id);
  };

  // return the value at a certain position.
  const T get(const size_t i) const {
    const ValueID id = ValueID(_attribute_vector->at(i));
    return _dictionary->at(id);  
    };

  // dictionary segments are immutable
  void append(const AllTypeVariant&) override {
    throw std::runtime_error("Can not append to full chunk");
  };

  // returns an underlying dictionary
  std::shared_ptr<const std::vector<T>> dictionary() const {
    return _dictionary;
  };

  // returns an underlying data structure
  std::shared_ptr<const BaseAttributeVector> attribute_vector() const {
    return _attribute_vector;
  };

  // return the value represented by a given ValueID
  const T& value_by_value_id(ValueID value_id) const {
    return _dictionary->at(value_id);
  };

  // returns the first value ID that refers to a value >= the search value
  // returns INVALID_VALUE_ID if all values are smaller than the search value
  ValueID lower_bound(T value) const {
    const auto iterator = std::lower_bound(_dictionary->cbegin(), _dictionary->cend(), value);
    if (iterator == _dictionary->cend()) {
      return INVALID_VALUE_ID;
    }
    return ValueID(iterator - _dictionary->cend());
  }; 

  // same as lower_bound(T), but accepts an AllTypeVariant
  ValueID lower_bound(const AllTypeVariant& value) const {
    return lower_bound(type_cast<T>(value));
  };

  // returns the first value ID that refers to a value > the search value
  // returns INVALID_VALUE_ID if all values are smaller than or equal to the search value
  ValueID upper_bound(T value) const {
    const auto iterator = std::upper_bound(_dictionary->cbegin(), _dictionary->cend(), value);
    if (iterator == _dictionary->cend()) {
      return INVALID_VALUE_ID;
    }
    return ValueID(iterator - _dictionary->cend());
  };

  // same as upper_bound(T), but accepts an AllTypeVariant
  ValueID upper_bound(const AllTypeVariant& value) const {
    return upper_bound(type_cast<T>(value));
  };

  // return the number of unique_values (dictionary entries)
  size_t unique_values_count() const {
    return _dictionary->size();
  };

  // return the number of entries
  size_t size() const override {
    return _attribute_vector->size();
  };

 protected:
  std::shared_ptr<std::vector<T>> _dictionary;
  std::shared_ptr<std::vector<uint32_t>> _attribute_vector;
};

}  // namespace opossum
