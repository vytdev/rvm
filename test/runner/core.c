#include "internal.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

struct test_entry *test__reg_head = NULL;
struct test_entry *test__reg_tail = NULL;


void test__register(test_fn fn, char const *name) {
  if (!fn || !name)
    return;
  /* Allocate memory for the item. */
  struct test_entry *st = (struct test_entry*)malloc(sizeof(struct test_entry));
  if (!st) {
    fprintf(stderr, "Out of memory\n");
    exit(1);
  }
  /* Set some fields. */
  st->func = fn;
  st->name = name;
  st->next = NULL;
  if (!test__reg_head)
    test__reg_head = st;
  if (test__reg_tail)
    test__reg_tail->next = st;
  test__reg_tail = st;
  /* Update states. */
  test__total_regs++;
}


void test__run(struct test_entry *st) {
  if (!st || !st->func || !st->name)
    return;
  test__vlog("Running test: %s\n", st->name);
  uint64_t elapsed = 0;
  test__has_failed = 0;

  /* Setup and run the test. */
  if (!test__opts_no_timer)
    test__timer_start();

  st->func();

  if (!test__opts_no_timer)
    elapsed = test__timer_mark();

  test__total_ran++;
  if (!test__has_failed)
    test__total_pass++;
  else
    test__total_fail++;

  /* Update timers. */
  test__total_time_ms += elapsed / MILLION;

  /* Some logging. */
  if (!test__opts_no_names)
    test__print_stat(st->name, test__has_failed, elapsed);

  /* Fast fail. */
  if (test__has_failed && test__opts_fast_fail)
    test__exit();
}


static void print_name(char const *name) {
  if (test__opts_no_color) {
    printf("%s", name);
    return;
  }
  /* Print colored name. */
  printf("\033[2m");
  while (*name != ':') {
    fputc(*name, stdout);
    name++;
  }
  fputc(':', stdout);
  name++;
  printf("\033[0m%s", name);
}


void test__print_stat(char const *name, int stat, uint64_t elapsed_ns) {
  printf(stat
    ? (test__opts_no_color ? "FAIL" : "\033[41mFAIL\033[0m")
    : (test__opts_no_color ? "PASS" : "\033[42mPASS\033[0m"));

  /* Print name. */
  fputc(' ', stdout);
  print_name(name);

  /* Print the elapsed time. */
  if (!test__opts_no_timer) {
    fputc(' ', stdout);
    if (!test__opts_no_color)
      printf("\033[2m");
    printf("(%u ms)", (uint32_t)elapsed_ns / MILLION);
    if (!test__opts_no_color)
      printf("\033[0m");
  }

  fputc('\n', stdout);
}
