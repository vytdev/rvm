#include "runner/test.h"
#include "../src/util.h"

TEST(p64s_test) {
  TEST_LOG("test 0xff to be 255\n");
  EXPECT_EQ(p64s("0xff",   0, NULL), 255);
  TEST_LOG("test 0b1101 to be 13\n");
  EXPECT_EQ(p64s("0b1101", 0, NULL), 13);
  TEST_LOG("test 0777 to be 511\n");
  EXPECT_EQ(p64s("0777",   0, NULL), 511);
  TEST_LOG("test 123 to be 123\n");
  EXPECT_EQ(p64s("123",    0, NULL), 123);
  TEST_END();
}

TEST(pdatasz_test) {
  TEST_LOG("test 1M to be 1,048,576\n");
  EXPECT_EQ(pdatasz("1M"), 1048576);
  TEST_LOG("test 1m to be 1,000,000\n");
  EXPECT_EQ(pdatasz("1m"), 1000000);
  TEST_END();
}

void register_tests__util(void) {
  TEST_REGISTER(p64s_test);
  TEST_REGISTER(pdatasz_test);
}
