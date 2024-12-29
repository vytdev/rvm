#include "internal.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

bool test__opts_help      = false;
bool test__opts_list      = false;
bool test__opts_verbose   = false;
bool test__opts_fast_fail = false;
bool test__opts_no_names  = false;
bool test__opts_no_color  = false;
bool test__opts_no_timer  = false;


/* Test registration function. */
void register_all_tests(void);

/* Show help msg and exit. */
void show_help(char const *prog);


int main(int argc, char **argv) {
  int i = 1;

  /* Parse options. */
  for ( ; i < argc; i++) {
    char *arg = argv[i];

    if (arg[0] != '-')
      break;

    if (arg[1] == '-' && strlen(arg) == 2) {
      i++;
      break;
    }

    for (int j = 1; j < strlen(arg); j++) {
      switch (arg[j]) {
        case 'h': test__opts_help      = true; break;
        case 'l': test__opts_list      = true; break;
        case 'v': test__opts_verbose   = true; break;
        case 'f': test__opts_fast_fail = true; break;
        case 'n': test__opts_no_names  = true; break;
        case 'c': test__opts_no_color  = true; break;
        case 't': test__opts_no_timer  = true; break;
        default:
          fprintf(stderr, "unknown option: -%c\n", arg[j]);
          return 1;
      }
    }
  }

  /* Print help. */
  if (test__opts_help) {
    show_help(argv[0]);
    return 1;
  }

  /* Register all tests. */
  register_all_tests();

  /* Print list. */
  if (test__opts_list) {
    struct test_entry *curr = test__reg_head;
    while (curr) {
      fprintf(stderr, "* %s\n", curr->name);
      curr = curr->next;
    }
    return 1;
  }

  /* Execute tests from here. */
  struct test_entry *curr = test__reg_head;
  while (curr) {
    int match = 0;
    if (i != argc)
      for (int j = i; j < argc; j++) {
        if (test__match(argv[j], curr->name)) {
          match = 1;
          break;
        }
      }
    else
      match = 1;
    if (match == 1)
      test__run(curr);
    curr = curr->next;
  }

  test__exit();
}


void show_help(char const *prog) {
  fprintf(stderr, "usage: %s [options] [tests...]\n", prog);
  fprintf(stderr,
      "options:\n"
      "  -h    Show this help and exit.\n"
      "  -l    Show the registry list.\n"
      "  -v    Enable test logging.\n"
      "  -f    Fail immediately.\n"
      "  -n    Don't log progress names.\n"
      "  -c    Don't colorise the output.\n"
      "  -t    Suppress timing output.\n"
      "RVM test runner\n"
    );
}
