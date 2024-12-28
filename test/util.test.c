#include "runner/test.h"
#include "../src/util.h"

TEST(p64s_test) {
  EXPECT_EQ(p64s("0xff",   0, NULL), 255);
  EXPECT_EQ(p64s("0b1101", 0, NULL), 13);
  EXPECT_EQ(p64s("0777",   0, NULL), 511);
  EXPECT_EQ(p64s("123",    0, NULL), 123);
  TEST_END();
}

TEST(pdatasz_test) {
  EXPECT_EQ(pdatasz("1M"), 1048576);
  EXPECT_EQ(pdatasz("1m"), 1000000);
  TEST_END();
}

void register_tests__util(void) {
  TEST_REGISTER(p64s_test);
  TEST_REGISTER(pdatasz_test);
}
