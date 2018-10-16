#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "../lib/storage/table.hpp"
#include "../lib/storage/value_segment.hpp"
#include "../lib/types.hpp"

#include "gtest/gtest.h"

namespace opossum {

class AbstractASTNode;
class Table;

using Matrix = std::vector<std::vector<AllTypeVariant>>;

class BaseTest : public ::testing::Test {
  using Matrix = std::vector<std::vector<AllTypeVariant>>;

  // helper functions for _table_equal
  static BaseTest::Matrix _table_to_matrix(const Table& table);
  static void _print_matrix(const BaseTest::Matrix& matrix);

  // helper function for load_table
  template <typename T>
  static std::vector<T> _split(const std::string& str, char delimiter);

 protected:
  // compares two tables with regard to the schema and content
  // but ignores the internal representation (chunk size, data type)
  static ::testing::AssertionResult _table_equal(const Table& tleft, const Table& tright, bool order_sensitive = false,
                                                 bool strict_types = true);

  // creates a opossum table based from a file
  static void EXPECT_TABLE_EQ(const Table& tleft, const Table& tright, bool order_sensitive = false,
                              bool strict_types = true);
  static void ASSERT_TABLE_EQ(const Table& tleft, const Table& tright, bool order_sensitive = false,
                              bool strict_types = true);

  static void EXPECT_TABLE_EQ(std::shared_ptr<const Table> tleft, std::shared_ptr<const Table> tright,
                              bool order_sensitive = false, bool strict_types = true);
  static void ASSERT_TABLE_EQ(std::shared_ptr<const Table> tleft, std::shared_ptr<const Table> tright,
                              bool order_sensitive = false, bool strict_types = true);

 public:
  virtual ~BaseTest();
};

}  // namespace opossum
