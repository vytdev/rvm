#include "../util.h"
#include "../../rvm/config.h"
#include "parser.h"

int main(int argc, char **argv) {
  main_argc = argc;
  main_argv = argv;

  if (argc < 2) {
    rlog("Please provide a file.\n");
    return 1;
  }

  uvar sz = 0;
  char *txt = util_readtext(argv[1], &sz);

  if (!txt) {
    rlog("Failed to load file: %s\n", argv[1]);
    return 1;
  }

  Parser ptx;
  if (!parser_init(&ptx, txt, argv[1])) {
    rlog("Out of memory.\n");
    return 1;
  }

  while (!ptx.stop) {
    Token *tok = parser_next(&ptx);
    if (!tok) {
      rlog("Token parsing error.\n");
      return 1;
    }
    parser_printToken(&ptx, tok, "token type: %d\n", tok->type);
  }

  parser_free(&ptx);

  return 0;
}
