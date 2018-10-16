#pragma once

#include <boost/hana/equal.hpp>
#include <boost/hana/for_each.hpp>
#include <boost/hana/size.hpp>

#include <functional>
#include <memory>
#include <string>
#include <utility>

#include "all_type_variant.hpp"
#include "utils/assert.hpp"

#include "storage/value_segment.hpp"

namespace opossum {

namespace hana = boost::hana;

/**
 * Resolves a type string by creating an instance of a templated class and
 * returning it as a unique_ptr of its non-templated base class.
 *
 * @param type is a string representation of any of the supported data types
 * @param args is a list of constructor arguments
 *
 *
 * Example:
 *
 *   class BaseImpl {
 *    public:
 *     virtual void execute() = 0;
 *   };
 *
 *   template <typename T>
 *   class Impl : public BaseImpl {
 *    public:
 *     Impl(int var) : _var{var} { ... }
 *
 *     void execute() override { ... }
 *   };
 *
 *   constexpr auto var = 12;
 *   auto impl = make_unique_by_data_type<BaseImpl, Impl>("string", var);
 *   impl->execute();
 */
template <class Base, template <typename...> class Impl, class... TemplateArgs, typename... ConstructorArgs>
std::unique_ptr<Base> make_unique_by_data_type(const std::string& type, ConstructorArgs&&... args) {
  std::unique_ptr<Base> ret = nullptr;
  hana::for_each(data_types, [&](auto x) {
    if (std::string(hana::first(x)) == type) {
      // The + before hana::second - which returns a reference - converts its return value
      // into a value so that we can access ::type
      using DataType = typename decltype(+hana::second(x))::type;
      ret = std::make_unique<Impl<DataType, TemplateArgs...>>(std::forward<ConstructorArgs>(args)...);
      return;
    }
  });
  DebugAssert(static_cast<bool>(ret), "unknown type " + type);
  return ret;
}

/**
 * Convenience function. Calls make_unique_by_data_type and casts the result into a shared_ptr.
 */
template <class Base, template <typename...> class impl, class... TemplateArgs, class... ConstructorArgs>
std::shared_ptr<Base> make_shared_by_data_type(const std::string& type, ConstructorArgs&&... args) {
  return make_unique_by_data_type<Base, impl, TemplateArgs...>(type, std::forward<ConstructorArgs>(args)...);
}

/**
 * Resolves a type string by passing a hana::type object on to a generic lambda
 *
 * @param type_string is a string representation of any of the supported data types
 * @param func is a generic lambda or similar accepting a hana::type object
 *
 *
 * Note on hana::type (taken from Boost.Hana documentation):
 *
 * For subtle reasons having to do with ADL, the actual representation of hana::type is
 * implementation-defined. In particular, hana::type may be a dependent type, so one
 * should not attempt to do pattern matching on it. However, one can assume that hana::type
 * inherits from hana::basic_type, which can be useful when declaring overloaded functions.
 *
 * This means that we need to use hana::basic_type as a parameter in methods so that the
 * underlying type can be deduced from the object.
 *
 *
 * Note on generic lambdas (taken from paragraph 5.1.2/5 of the C++14 Standard Draft n3690):
 *
 * For a generic lambda, the closure type has a public inline function call operator member template (14.5.2)
 * whose template-parameter-list consists of one invented type template-parameter for each occurrence of auto
 * in the lambda’s parameter-declaration-clause, in order of appearance. Example:
 *
 *   auto lambda = [] (auto a) { return a; };
 *
 *   class // unnamed {
 *    public:
 *     template<typename T>
 *     auto operator()(T a) const { return a; }
 *   };
 *
 *
 * Example:
 *
 *   template <typename T>
 *   process_variant(const T& var);
 *
 *   template <typename T>
 *   process_type(hana::basic_type<T> type);  // note: parameter type needs to be hana::basic_type not hana::type!
 *
 *   resolve_data_type(type_string, [&](auto type) {
 *     using Type = typename decltype(type)::type;
 *     const auto var = type_cast<Type>(variant_from_elsewhere);
 *     process_variant(var);
 *
 *     process_type(type);
 *   });
 */
template <typename Functor>
void resolve_data_type(const std::string& type_string, const Functor& func) {
  hana::for_each(data_types, [&](auto x) {
    if (std::string(hana::first(x)) == type_string) {
      // The + before hana::second - which returns a reference - converts its return value into a value
      func(+hana::second(x));
      return;
    }
  });
}

}  // namespace opossum
