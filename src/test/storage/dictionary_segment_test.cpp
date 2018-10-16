#include <memory>
#include <string>

#include "gtest/gtest.h"

#include "../../lib/resolve_type.hpp"
#include "../../lib/storage/base_segment.hpp"
#include "../../lib/storage/dictionary_segment.hpp"
#include "../../lib/storage/value_segment.hpp"

// class StorageDictionarySegmentTest : public ::testing::Test {
//  protected:
//   std::shared_ptr<opossum::ValueSegment<int>> int_value_segment = std::make_shared<opossum::ValueSegment<int>>();
//   std::shared_ptr<opossum::ValueSegment<std::string>> string_value_segment = std::make_shared<opossum::ValueSegment<std::string>>();
// };

// TEST_F(StorageDictionarySegmentTest, CompressStringSegment) {
//   string_value_segment->append("Bill");
//   string_value_segment->append("Steve");
//   string_value_segment->append("Alexander");
//   string_value_segment->append("Steve");
//   string_value_segment->append("Hasso");
//   string_value_segment->append("Bill");

//   auto base_segment = opossum::make_shared_by_data_type<opossum::BaseSegment, opossum::DictionarySegment>("string", string_value_segment);
//   auto dictionary_segment = std::dynamic_pointer_cast<opossum::DictionarySegment<std::string>>(base_segment);

//   // Test attribute_vector size
//   EXPECT_EQ(dictionary_segment->size(), 6u);

//   // Test dictionary size (uniqueness)
//   EXPECT_EQ(dictionary_segment->unique_values_count(), 4u);

//   // Test sorting
//   auto dict = dictionary_segment->dictionary();
//   EXPECT_EQ((*dict)[0], "Alexander");
//   EXPECT_EQ((*dict)[1], "Bill");
//   EXPECT_EQ((*dict)[2], "Hasso");
//   EXPECT_EQ((*dict)[3], "Steve");
// }

// TEST_F(StorageDictionarySegmentTest, LowerUpperBound) {
//   for (int i = 0; i <= 10; i += 2) int_value_segment->append(i);
//   auto base_segment = opossum::make_shared_by_data_type<opossum::BaseSegment, opossum::DictionarySegment>("int", int_value_segment);
//   auto dictionary_segment = std::dynamic_pointer_cast<opossum::DictionarySegment<int>>(base_segment);

//   EXPECT_EQ(dictionary_segment->lower_bound(4), (opossum::ValueID)2);
//   EXPECT_EQ(dictionary_segment->upper_bound(4), (opossum::ValueID)3);

//   EXPECT_EQ(dictionary_segment->lower_bound(5), (opossum::ValueID)3);
//   EXPECT_EQ(dictionary_segment->upper_bound(5), (opossum::ValueID)3);

//   EXPECT_EQ(dictionary_segment->lower_bound(15), opossum::INVALID_VALUE_ID);
//   EXPECT_EQ(dictionary_segment->upper_bound(15), opossum::INVALID_VALUE_ID);
// }

// TODO(student): You should add some more tests here (full coverage would be appreciated) and possibly in other files.
