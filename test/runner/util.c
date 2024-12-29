#include "internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <time.h>

int test__total_regs    = 0;
int test__total_ran     = 0;
int test__total_pass    = 0;
int test__total_fail    = 0;
int test__total_time_ms = 0;
uint64_t test__timer    = 0;
int test__has_failed    = 0;

void test__exit(void) {
  fputc('\n', stdout);
  printf("Total: %d\n", test__total_regs);
  printf("Ran:   %d\n", test__total_ran);
  printf("Pass:  %d\n", test__total_pass);
  printf("Fail:  %d\n", test__total_fail);
  printf("Skip:  %d\n", test__total_regs - test__total_ran);
  if (!test__opts_no_timer)
    printf("Took %d ms.\n", test__total_time_ms);
  exit(test__total_fail > 0 ? 1 : 0);
}


void test__timer_start(void) {
  if (test__opts_no_timer)
    return;
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  test__timer = CALC_NS(ts);
}


uint64_t test__timer_mark(void) {
  if (test__opts_no_timer)
    return 0;
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return CALC_NS(ts) - test__timer;
}


void test__vlog(char const *fmt, ...) {
  if (!test__opts_verbose)
    return;
  printf("> ");
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
}


void test__vlog_noprefix(char const *fmt, ...) {
  if (!test__opts_verbose)
    return;
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
}


int test__strcmp(char *a, char *b) {
  while (1) {
    int cmp = *a - *b;
    if (cmp != 0) return cmp;
    if (*a == '\0') return 0;
    a++;
    b++;
  }
}


int test__strcasecmp(char *a, char *b) {
  #define is_lower_alpha(c) ((c) >= 'a' && (c) <= 'z')
  #define to_upper(c) ((c) - 'a' + 'A')

  while (1) {
    char ca = *a;
    char cb = *b;

    if (is_lower_alpha(ca))
      ca = to_upper(ca);

    if (is_lower_alpha(cb))
      cb = to_upper(cb);

    int cmp = ca - cb;
    if (cmp != 0) return cmp;
    if (*a == '\0') return 0;
    a++;
    b++;
  }

  #undef is_lower_alpha
  #undef to_upper
}


int test__match(char *p, char const *str) {
  if (!p || !str)
    return 0;
  while (*p != '\0') {
    /* Process escapes. */
    if (*p == '\\') {
      p++;
      if (*p == '\0' || *p++ != *str++)
        return 0;
      continue;
    }
    /* Process '?' wildcard. */
    if (*p == '?') {
      p++;
      str++;
      continue;
    }
    /* Process '*' wildcard. */
    if (*p == '*') {
      p++;
      if (*p == '\0')
        return 1;
      while (*str != '\0')
        if (test__match(p, str++))
          return 1;
      return 0;
    }
    /* Char match. */
    if (*str != '\0' && *p++ == *str++)
      continue;
    return 0;
  }
  return *str == '\0';
}
