#pragma once

#include <boost/hana/ext/boost/mpl/vector.hpp>
#include <boost/hana/pair.hpp>
#include <boost/hana/prepend.hpp>
#include <boost/hana/second.hpp>
#include <boost/hana/transform.hpp>
#include <boost/hana/tuple.hpp>
#include <boost/hana/zip.hpp>
#include <boost/mpl/push_front.hpp>
#include <boost/variant.hpp>

#include <boost/preprocessor/seq/enum.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/transform.hpp>

#include <cstdint>
#include <string>
#include <vector>

#include "types.hpp"

namespace opossum {

namespace hana = boost::hana;

namespace detail {

#define EXPAND_TO_HANA_TYPE(s, data, elem) boost::hana::type_c<elem>

// clang-format off
#define data_types_macro                              (int32_t) (int64_t) (float)  (double)  (std::string)    // NOLINT
static constexpr auto type_strings = hana::make_tuple("int",    "long",   "float", "double", "string"     );  // NOLINT
// clang-format on

// Extends to hana::make_tuple(hana::type_c<int32_t>, hana::type_c<int64_t>, ...);
static constexpr auto types =
    hana::make_tuple(BOOST_PP_SEQ_ENUM(BOOST_PP_SEQ_TRANSFORM(EXPAND_TO_HANA_TYPE, _, data_types_macro)));

/**
 * Holds pairs of all types and their respective string representation.
 *
 * Equivalent to:
 * hana::make_tuple(hana::make_tuple("int", hana::type_c<int32_t>),
 *                  hana::make_tuple("long", hana::type_c<int64_t>),
 *                  ...);
 */
static constexpr auto data_types_as_tuples = hana::zip(type_strings, types);

struct to_pair {
  template <typename T>
  constexpr auto operator()(T tuple) {
    return hana::make_pair(hana::at_c<0>(tuple), hana::at_c<1>(tuple));  // NOLINT
  }
};

// Converts the tuples into pairs
static constexpr auto data_types = hana::transform(data_types_as_tuples, to_pair{});  // NOLINT

// Converts tuple to mpl vector
using TypesAsMplVector = decltype(hana::to<hana::ext::boost::mpl::vector_tag>(types));

// Creates boost::variant from mpl vector
using AllTypeVariant = typename boost::make_variant_over<detail::TypesAsMplVector>::type;

}  // namespace detail

static constexpr auto types = detail::types;
static constexpr auto data_types = detail::data_types;

using AllTypeVariant = detail::AllTypeVariant;

/**
 * @defgroup Macros for explicitly instantiating template classes
 *
 * In order to improve compile times, we explicitly instantiate
 * template classes which are going to be used with data types.
 * Because we do not want any redundant lists of data types spread
 * across the code base, we use EXPLICITLY_INSTANTIATE_DATA_TYPES.
 *
 * @{
 */

#define EXPLICIT_INSTANTIATION(r, template_class, type) template class template_class<type>;

// Explicitly instantiates the given template class for all types in data_types
#define EXPLICITLY_INSTANTIATE_DATA_TYPES(template_class)                         \
  BOOST_PP_SEQ_FOR_EACH(EXPLICIT_INSTANTIATION, template_class, data_types_macro) \
  static_assert(true, "End call of macro with a semicolon")

/**@}*/

}  // namespace opossum
