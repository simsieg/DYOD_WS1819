#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "../base_test.hpp"
#include "gtest/gtest.h"

#include "operators/print.hpp"
#include "operators/table_scan.hpp"
#include "operators/table_wrapper.hpp"
#include "storage/reference_segment.hpp"
#include "storage/table.hpp"
#include "types.hpp"
#include "utils/load_table.hpp"

namespace opossum {

// class OperatorsTableScanTest : public BaseTest {
//  protected:
//   void SetUp() override {
//     _table_wrapper = std::make_shared<TableWrapper>(load_table("src/test/tables/int_float.tbl", 2));
//     _table_wrapper->execute();

//     std::shared_ptr<Table> test_even_dict = std::make_shared<Table>(5);
//     test_even_dict->add_column("a", "int");
//     test_even_dict->add_column("b", "int");
//     for (int i = 0; i <= 24; i += 2) test_even_dict->append({i, 100 + i});

//     test_even_dict->compress_chunk(ChunkID(0));
//     test_even_dict->compress_chunk(ChunkID(1));

//     _table_wrapper_even_dict = std::make_shared<TableWrapper>(std::move(test_even_dict));
//     _table_wrapper_even_dict->execute();
//   }

//   std::shared_ptr<TableWrapper> get_table_op_part_dict() {
//     auto table = std::make_shared<Table>(5);
//     table->add_column("a", "int");
//     table->add_column("b", "float");

//     for (int i = 1; i < 20; ++i) {
//       table->append({i, 100.1 + i});
//     }

//     table->compress_chunk(ChunkID(0));
//     table->compress_chunk(ChunkID(1));

//     auto table_wrapper = std::make_shared<TableWrapper>(table);
//     table_wrapper->execute();

//     return table_wrapper;
//   }

//   std::shared_ptr<TableWrapper> get_table_op_with_n_dict_entries(const int num_entries) {
//     // Set up dictionary encoded table with a dictionary consisting of num_entries entries.
//     auto table = std::make_shared<opossum::Table>(0);
//     table->add_column("a", "int");
//     table->add_column("b", "float");

//     for (int i = 0; i <= num_entries; i++) {
//       table->append({i, 100.0f + i});
//     }

//     table->compress_chunk(ChunkID(0));

//     auto table_wrapper = std::make_shared<opossum::TableWrapper>(std::move(table));
//     table_wrapper->execute();
//     return table_wrapper;
//   }

//   void ASSERT_COLUMN_EQ(std::shared_ptr<const Table> table, const ColumnID& column_id,
//                         std::vector<AllTypeVariant> expected) {
//     for (auto chunk_id = ChunkID{0u}; chunk_id < table->chunk_count(); ++chunk_id) {
//       const auto& chunk = table->get_chunk(chunk_id);

//       for (auto chunk_offset = ChunkOffset{0u}; chunk_offset < chunk.size(); ++chunk_offset) {
//         const auto& segment = *chunk.get_segment(column_id);

//         const auto found_value = segment[chunk_offset];
//         const auto comparator = [found_value](const AllTypeVariant expected_value) {
//           // returns equivalency, not equality to simulate std::multiset.
//           // multiset cannot be used because it triggers a compiler / lib bug when built in CI
//           return !(found_value < expected_value) && !(expected_value < found_value);
//         };

//         auto search = std::find_if(expected.begin(), expected.end(), comparator);

//         ASSERT_TRUE(search != expected.end());
//         expected.erase(search);
//       }
//     }

//     ASSERT_EQ(expected.size(), 0u);
//   }

//   std::shared_ptr<TableWrapper> _table_wrapper, _table_wrapper_even_dict;
// };

// TEST_F(OperatorsTableScanTest, DoubleScan) {
//   std::shared_ptr<Table> expected_result = load_table("src/test/tables/int_float_filtered.tbl", 2);

//   auto scan_1 = std::make_shared<TableScan>(_table_wrapper, ColumnID{0}, ScanType::OpGreaterThanEquals, 1234);
//   scan_1->execute();

//   auto scan_2 = std::make_shared<TableScan>(scan_1, ColumnID{1}, ScanType::OpLessThan, 457.9);
//   scan_2->execute();

//   EXPECT_TABLE_EQ(scan_2->get_output(), expected_result);
// }

// TEST_F(OperatorsTableScanTest, EmptyResultScan) {
//   auto scan_1 = std::make_shared<TableScan>(_table_wrapper, ColumnID{0}, ScanType::OpGreaterThan, 90000);
//   scan_1->execute();

//   for (auto i = ChunkID{0}; i < scan_1->get_output()->chunk_count(); i++)
//     EXPECT_EQ(scan_1->get_output()->get_chunk(i).column_count(), 2u);
// }

// TEST_F(OperatorsTableScanTest, SingleScanReturnsCorrectRowCount) {
//   std::shared_ptr<Table> expected_result = load_table("src/test/tables/int_float_filtered2.tbl", 1);

//   auto scan = std::make_shared<TableScan>(_table_wrapper, ColumnID{0}, ScanType::OpGreaterThanEquals, 1234);
//   scan->execute();

//   EXPECT_TABLE_EQ(scan->get_output(), expected_result);
// }

// TEST_F(OperatorsTableScanTest, ScanOnDictColumn) {
//   // we do not need to check for a non existing value, because that happens automatically when we scan the second chunk

//   std::map<ScanType, std::vector<AllTypeVariant>> tests;
//   tests[ScanType::OpEquals] = {104};
//   tests[ScanType::OpNotEquals] = {100, 102, 106, 108, 110, 112, 114, 116, 118, 120, 122, 124};
//   tests[ScanType::OpLessThan] = {100, 102};
//   tests[ScanType::OpLessThanEquals] = {100, 102, 104};
//   tests[ScanType::OpGreaterThan] = {106, 108, 110, 112, 114, 116, 118, 120, 122, 124};
//   tests[ScanType::OpGreaterThanEquals] = {104, 106, 108, 110, 112, 114, 116, 118, 120, 122, 124};
//   for (const auto& test : tests) {
//     auto scan = std::make_shared<TableScan>(_table_wrapper_even_dict, ColumnID{0}, test.first, 4);
//     scan->execute();

//     ASSERT_COLUMN_EQ(scan->get_output(), ColumnID{1}, test.second);
//   }
// }

// TEST_F(OperatorsTableScanTest, ScanOnReferencedDictColumn) {
//   // we do not need to check for a non existing value, because that happens automatically when we scan the second chunk

//   std::map<ScanType, std::vector<AllTypeVariant>> tests;
//   tests[ScanType::OpEquals] = {104};
//   tests[ScanType::OpNotEquals] = {100, 102, 106};
//   tests[ScanType::OpLessThan] = {100, 102};
//   tests[ScanType::OpLessThanEquals] = {100, 102, 104};
//   tests[ScanType::OpGreaterThan] = {106};
//   tests[ScanType::OpGreaterThanEquals] = {104, 106};
//   for (const auto& test : tests) {
//     auto scan1 = std::make_shared<TableScan>(_table_wrapper_even_dict, ColumnID{1}, ScanType::OpLessThan, 108);
//     scan1->execute();

//     auto scan2 = std::make_shared<TableScan>(scan1, ColumnID{0}, test.first, 4);
//     scan2->execute();

//     ASSERT_COLUMN_EQ(scan2->get_output(), ColumnID{1}, test.second);
//   }
// }

// TEST_F(OperatorsTableScanTest, ScanPartiallyCompressed) {
//   std::shared_ptr<Table> expected_result = load_table("src/test/tables/int_float_seq_filtered.tbl", 2);

//   auto table_wrapper = get_table_op_part_dict();
//   auto scan_1 = std::make_shared<TableScan>(table_wrapper, ColumnID{0}, ScanType::OpLessThan, 10);
//   scan_1->execute();

//   EXPECT_TABLE_EQ(scan_1->get_output(), expected_result);
// }

// TEST_F(OperatorsTableScanTest, ScanOnDictColumnValueGreaterThanMaxDictionaryValue) {
//   const auto all_rows = std::vector<AllTypeVariant>{100, 102, 104, 106, 108, 110, 112, 114, 116, 118, 120, 122, 124};
//   const auto no_rows = std::vector<AllTypeVariant>{};

//   std::map<ScanType, std::vector<AllTypeVariant>> tests;
//   tests[ScanType::OpEquals] = no_rows;
//   tests[ScanType::OpNotEquals] = all_rows;
//   tests[ScanType::OpLessThan] = all_rows;
//   tests[ScanType::OpLessThanEquals] = all_rows;
//   tests[ScanType::OpGreaterThan] = no_rows;
//   tests[ScanType::OpGreaterThanEquals] = no_rows;

//   for (const auto& test : tests) {
//     auto scan = std::make_shared<TableScan>(_table_wrapper_even_dict, ColumnID{0}, test.first, 30);
//     scan->execute();

//     ASSERT_COLUMN_EQ(scan->get_output(), ColumnID{1}, test.second);
//   }
// }

// TEST_F(OperatorsTableScanTest, ScanOnDictColumnValueLessThanMinDictionaryValue) {
//   const auto all_rows = std::vector<AllTypeVariant>{100, 102, 104, 106, 108, 110, 112, 114, 116, 118, 120, 122, 124};
//   const auto no_rows = std::vector<AllTypeVariant>{};

//   std::map<ScanType, std::vector<AllTypeVariant>> tests;
//   tests[ScanType::OpEquals] = no_rows;
//   tests[ScanType::OpNotEquals] = all_rows;
//   tests[ScanType::OpLessThan] = no_rows;
//   tests[ScanType::OpLessThanEquals] = no_rows;
//   tests[ScanType::OpGreaterThan] = all_rows;
//   tests[ScanType::OpGreaterThanEquals] = all_rows;

//   for (const auto& test : tests) {
//     auto scan = std::make_shared<TableScan>(_table_wrapper_even_dict, ColumnID{0} /* "a" */, test.first, -10);
//     scan->execute();

//     ASSERT_COLUMN_EQ(scan->get_output(), ColumnID{1}, test.second);
//   }
// }

// TEST_F(OperatorsTableScanTest, ScanOnDictColumnAroundBounds) {
//   // scanning for a value that is around the dictionary's bounds

//   std::map<ScanType, std::vector<AllTypeVariant>> tests;
//   tests[ScanType::OpEquals] = {100};
//   tests[ScanType::OpLessThan] = {};
//   tests[ScanType::OpLessThanEquals] = {100};
//   tests[ScanType::OpGreaterThan] = {102, 104, 106, 108, 110, 112, 114, 116, 118, 120, 122, 124};
//   tests[ScanType::OpGreaterThanEquals] = {100, 102, 104, 106, 108, 110, 112, 114, 116, 118, 120, 122, 124};
//   tests[ScanType::OpNotEquals] = {102, 104, 106, 108, 110, 112, 114, 116, 118, 120, 122, 124};

//   for (const auto& test : tests) {
//     auto scan = std::make_shared<opossum::TableScan>(_table_wrapper_even_dict, ColumnID{0}, test.first, 0);
//     scan->execute();

//     ASSERT_COLUMN_EQ(scan->get_output(), ColumnID{1}, test.second);
//   }
// }

// TEST_F(OperatorsTableScanTest, ScanWithEmptyInput) {
//   auto scan_1 = std::make_shared<opossum::TableScan>(_table_wrapper, ColumnID{0}, ScanType::OpGreaterThan, 12345);
//   scan_1->execute();
//   EXPECT_EQ(scan_1->get_output()->row_count(), static_cast<size_t>(0));

//   // scan_1 produced an empty result
//   auto scan_2 = std::make_shared<opossum::TableScan>(scan_1, ColumnID{1}, ScanType::OpEquals, 456.7);
//   scan_2->execute();

//   EXPECT_EQ(scan_2->get_output()->row_count(), static_cast<size_t>(0));
// }

// TEST_F(OperatorsTableScanTest, ScanOnWideDictionarySegment) {
//   // 2**8 + 1 values require a data type of 16bit.
//   const auto table_wrapper_dict_16 = get_table_op_with_n_dict_entries((1 << 8) + 1);
//   auto scan_1 = std::make_shared<opossum::TableScan>(table_wrapper_dict_16, ColumnID{0}, ScanType::OpGreaterThan, 200);
//   scan_1->execute();

//   EXPECT_EQ(scan_1->get_output()->row_count(), static_cast<size_t>(57));

//   // 2**16 + 1 values require a data type of 32bit.
//   const auto table_wrapper_dict_32 = get_table_op_with_n_dict_entries((1 << 16) + 1);
//   auto scan_2 =
//       std::make_shared<opossum::TableScan>(table_wrapper_dict_32, ColumnID{0}, ScanType::OpGreaterThan, 65500);
//   scan_2->execute();

//   EXPECT_EQ(scan_2->get_output()->row_count(), static_cast<size_t>(37));
// }

}  // namespace opossum
