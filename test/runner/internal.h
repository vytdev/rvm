#ifndef TEST_INTERNAL_H_
#define TEST_INTERNAL_H_
#include <stdbool.h>
#include <stdint.h>

#ifndef __linux__
#  error "Unsupported OS."
#endif

/* Test CLI options. */
extern bool test__opts_help;
extern bool test__opts_list;
extern bool test__opts_verbose;
extern bool test__opts_fast_fail;
extern bool test__opts_no_names;
extern bool test__opts_no_color;
extern bool test__opts_no_timer;

/* Testing states. */
extern int test__total_regs;
extern int test__total_ran;
extern int test__total_pass;
extern int test__total_fail;
extern int test__total_time_ms;
extern uint64_t test__timer;
extern int test__has_failed;

/* The test registry. */
typedef void(*test_fn)(void);
struct test_entry {
  char const *name;
  test_fn    func;
  struct test_entry *next;
};

extern struct test_entry *test__reg_head;
extern struct test_entry *test__reg_tail;


/* Register a test. */
void test__register(test_fn fn, char const *name);

/* Run a test by test struct. */
void test__run(struct test_entry *st);

/* Print a test run stat. */
void test__print_stat(char const *name, int stat, uint64_t elapsed_ns);

/* Glob-like matching. */
int test__match(char *p, char const *str);

/* Exit test with logging. */
void test__exit(void);

/* Start the timer (update the last start epoch). */
void test__timer_start(void);

/* Get the current elapsed time. */
uint64_t test__timer_mark(void);

/* For verbose logging. */
void test__vlog(char const *fmt, ...);

/* Same as test__vlog(), but no prefix (> ). */
void test__vlog_noprefix(char const *fmt, ...);

/* strcmp() */
int test__strcmp(char *a, char *b);

/* strcasecmp() */
int test__strcasecmp(char *a, char *b);


/* Useful constants. */
#ifdef MILLION
#undef MILLION
#endif
#define MILLION 1000000

#ifdef BILLION
#undef BILLION
#endif
#define BILLION 1000000000

#ifdef CALC_NS
#undef CALC_NS
#endif
#define CALC_NS(ts) ((ts).tv_sec * BILLION + (ts).tv_nsec)

#endif // TEST_INTERNAL_H_
