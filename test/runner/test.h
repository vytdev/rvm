#ifndef TEST_H_
#define TEST_H_
#include "internal.h"


/* Defines a test function. */
#define TEST(fn) \
  void tfn__##fn (void)

/* End the test. */
#define TEST_END() do { \
    test__vlog("Test stopped.\n"); \
    return;             \
  } while (0)

/* Pass the test. */
#define TEST_PASS() do { \
    test__vlog("Test passed.\n"); \
    test__has_failed = 0;\
    return;              \
  } while (0)

/* Fail the test. */
#define TEST_FAIL() do { \
    test__vlog("Test failed.\n"); \
    test__has_failed = 1;\
    return;              \
  } while (0)

/* Register a test. */
#define TEST_REGISTER(fn) \
  (test__register(tfn__##fn, __FILE__ ":" #fn))

/* Log test infos. */
#define TEST_LOG(msg...) \
  (test__vlog(msg))

/* Continue the previous log. */
#define TEST_NLOG(msg...) \
  (test__vlog_noprefix(msg))


/* Assert expr to evaluate to true. */
#define ASSERT(expr, msg...) do { \
    if (!(expr)) {                \
      test__vlog("Assert on ln %d: ", __LINE__); \
      test__vlog_noprefix("" msg);\
      test__has_failed = 1;       \
      return;                     \
    } \
  } while (0)

/* Expect expr to evaluate to true. */
#define EXPECT(expr, msg...) do { \
    if (!(expr)) {                \
      test__vlog("Expect on ln %d: ", __LINE__); \
      test__vlog_noprefix("" msg);\
      test__has_failed = 1;       \
    }                             \
  } while (0)

#define ASSERT_TRUE(expr) ASSERT((expr), "expected "#expr" to be true\n")
#define EXPECT_TRUE(expr) EXPECT((expr), "expected "#expr" to be true\n")

#define ASSERT_FALSE(expr) ASSERT(!(expr), "expected "#expr" to be false\n")
#define EXPECT_FALSE(expr) EXPECT(!(expr), "expected "#expr" to be false\n")

#define ASSERT_EQ(a,b) ASSERT((a) == (b), "expected "#a" == "#b"\n")
#define EXPECT_EQ(a,b) EXPECT((a) == (b), "expected "#a" == "#b"\n")

#define ASSERT_NE(a,b) ASSERT((a) != (b), "expected "#a" != "#b"\n")
#define EXPECT_NE(a,b) EXPECT((a) != (b), "expected "#a" != "#b"\n")

#define ASSERT_GT(a,b) ASSERT((a) >  (b), "expected "#a" > "#b"\n")
#define EXPECT_GT(a,b) EXPECT((a) >  (b), "expected "#a" > "#b"\n")

#define ASSERT_GE(a,b) ASSERT((a) >= (b), "expected "#a" >= "#b"\n")
#define EXPECT_GE(a,b) EXPECT((a) >= (b), "expected "#a" >= "#b"\n")

#define ASSERT_LT(a,b) ASSERT((a) <  (b), "expected "#a" < "#b"\n")
#define EXPECT_LT(a,b) EXPECT((a) <  (b), "expected "#a" < "#b"\n")

#define ASSERT_LE(a,b) ASSERT((a) == (b), "expected "#a" <= "#b"\n")
#define EXPECT_LE(a,b) EXPECT((a) == (b), "expected "#a" <= "#b"\n")

#define ASSERT_STREQ(a,b) \
  ASSERT(test__strcmp((a),(b)) == 0, "expected string "#a" == "#b"\n" \
         "str1: %s\nstr2: %s\n", (a), (b))
#define EXPECT_STREQ(a,b) \
  EXPECT(test__strcmp((a),(b)) == 0, "expected string "#a" == "#b"\n" \
         "str1: %s\nstr2: %s\n", (a), (b))

#define ASSERT_STRNE(a,b) \
  ASSERT(test__strcmp((a),(b)) != 0, "expected string "#a" != "#b"\n" \
         "str1: %s\nstr2: %s\n", (a), (b))
#define EXPECT_STRNE(a,b) \
  EXPECT(test__strcmp((a),(b)) != 0, "expected string "#a" != "#b"\n" \
         "str1: %s\nstr2: %s\n", (a), (b))

#define ASSERT_STRCEQ(a,b) \
  ASSERT(test__strcasecmp((a),(b)) == 0, "expected string "#a" == "#b"\n" \
         "str1: %s\nstr2: %s\n", (a), (b))
#define EXPECT_STRCEQ(a,b) \
  EXPECT(test__strcasecmp((a),(b)) == 0, "expected string "#a" == "#b"\n" \
         "str1: %s\nstr2: %s\n", (a), (b))

#define ASSERT_STRCNE(a,b) \
  ASSERT(test__strcasecmp((a),(b)) != 0, "expected string "#a" != "#b"\n" \
         "str1: %s\nstr2: %s\n", (a), (b))
#define EXPECT_STRCNE(a,b) \
  EXPECT(test__strcasecmp((a),(b)) != 0, "expected string "#a" != "#b"\n" \
         "str1: %s\nstr2: %s\n", (a), (b))

#endif // TEST_H_
