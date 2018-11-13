#include <limits>
#include <memory>
#include <string>
#include <vector>

#include "../base_test.hpp"
#include "gtest/gtest.h"

#include "../lib/storage/fitted_attribute_vector.hpp"

namespace opossum {

class FittedAttributeVectorTest : public BaseTest {
 protected:
  std::shared_ptr<BaseAttributeVector> test_vector8 = std::make_shared<FittedAttributeVector<uint8_t>>();
  std::shared_ptr<BaseAttributeVector> test_vector16 = std::make_shared<FittedAttributeVector<uint16_t>>();
  std::shared_ptr<BaseAttributeVector> test_vector32 = std::make_shared<FittedAttributeVector<uint32_t>>();
  std::shared_ptr<BaseAttributeVector> test_vector64 = std::make_shared<FittedAttributeVector<uint64_t>>();
};

TEST_F(FittedAttributeVectorTest, GetWidth) {
  EXPECT_EQ(test_vector8->width(), AttributeVectorWidth(sizeof(uint8_t)));
  EXPECT_EQ(test_vector16->width(), AttributeVectorWidth(sizeof(uint16_t)));
  EXPECT_EQ(test_vector32->width(), AttributeVectorWidth(sizeof(uint32_t)));
  EXPECT_EQ(test_vector64->width(), AttributeVectorWidth(sizeof(uint64_t)));
}

TEST_F(FittedAttributeVectorTest, InitialSize) {
  EXPECT_EQ(test_vector8->size(), 0u);
  EXPECT_EQ(test_vector16->size(), 0u);
  EXPECT_EQ(test_vector32->size(), 0u);
  EXPECT_EQ(test_vector64->size(), 0u);
}

TEST_F(FittedAttributeVectorTest, BasicSetGet) {
  test_vector8->set(0, ValueID(20));
  EXPECT_EQ(test_vector8->get(0), 20u);
  test_vector16->set(0, ValueID(20));
  EXPECT_EQ(test_vector16->get(0), 20u);
  test_vector32->set(0, ValueID(20));
  EXPECT_EQ(test_vector32->get(0), 20u);
  test_vector64->set(0, ValueID(20));
  EXPECT_EQ(test_vector64->get(0), 20u);
}

}  // namespace opossum
