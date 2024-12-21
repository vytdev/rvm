#include "parser.h"
#include "../../rvm/config.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>


bool parser_init(Parser *ptx, char *src, char *file) {
  if (!ptx)
    return false;
  ptx->toks = (Token*)malloc(sizeof(Token) * 64);
  if (!ptx->toks)
    return false;
  ptx->tok_len   = 0;
  ptx->tok_alloc = 64;
  ptx->tokpos    = 0;
  ptx->file = file;
  ptx->src  = src;
  ptx->len  = strlen(src);
  ptx->pos  = 0;
  ptx->line = 1;
  ptx->col  = 1;
  ptx->stop = false;
  return true;
}


void parser_free(Parser *ptx) {
  if (!ptx)
    return;
  if (ptx->toks)
    free(ptx->toks);
  ptx->toks = NULL;
  ptx->tok_len   = 0;
  ptx->tok_alloc = 0;
  ptx->tokpos    = 0;
  ptx->file = NULL;
  ptx->src  = NULL;
  ptx->len  = 0;
  ptx->pos  = 0;
  ptx->line = 0;
  ptx->col  = 0;
  ptx->stop = false;
}


static bool parser__reallocToks(Parser *ptx) {
  if (!ptx)
    return false;
  ptx->toks = (Token*)realloc(ptx->toks,
     ptx->tok_alloc * 2 * sizeof(Token));
  if (!ptx->toks)
    return false;
  ptx->tok_alloc *= 2;
  return true;
}


static void parser__increment(Parser *ptx) {
  if (!ptx)
    return;
  switch (ptx->src[ptx->pos++]) {
    case ' ':
      ptx->col++;
      break;
    case '\t':
      ptx->col += 8 - (ptx->col % 8);
      break;
    case '\r':
      ptx->col = 1;
      break;
    case '\n':
      ptx->col = 1;
      ptx->line++;
      break;
    default:
      ptx->col++;
  }
}


Token *parser_tokenize(Parser *ptx) {
  if (!ptx)
    return NULL;
  if (ptx->stop)
    return &ptx->toks[ptx->tokpos-1];
  if (ptx->tok_len >= ptx->tok_alloc)
    if (!parser__reallocToks(ptx))
      return NULL;
  Token tok;
  #define ch (ptx->src[ptx->pos])
  #define tok_start(t) do { \
      tok.type = (t);       \
      tok.val  = &ptx->src[ptx->pos]; \
      tok.pos  = ptx->pos;  \
      tok.line = ptx->line; \
      tok.col  = ptx->col;  \
    } while (0)
  #define tok_len(n) (tok.len = (n))

  /* Skip whitespace. */
  while (isz_wsp(ch) && ch != '\n')
    parser__increment(ptx);

  /* Skip comments. */
  if (ch == ';')
    while (ch != '\n' && ch != '\0')
      parser__increment(ptx);

  /* EOF token. */
  if (ch == '\0') {
    tok_start(TOK_EOF);
    tok_len(1);
    ptx->stop = true;
  }

  /* Line token. */
  else if (ch == '\n') {
    tok_start(TOK_LINE);
    tok_len(1);
    parser__increment(ptx);
  }

  /* Comma token. */
  else if (ch == ',') {
    tok_start(TOK_COMMA);
    tok_len(1);
    parser__increment(ptx);
  }

  /* Text token. */
  else if (isz_alp(ch) || ch == '_') {
    tok_start(TOK_TEXT);
    pos_t len = 0;
    while (isz_idn(ch)) {
      parser__increment(ptx);
      len++;
    }
    if (ch == ':') {
      tok.type = TOK_LABEL;
      parser__increment(ptx);
      len++;
    }
    tok_len(len);
  }

  /* Register token. */
  else if (ch == '%') {
    tok_start(TOK_REG);
    parser__increment(ptx);
    pos_t len = 1;
    while (isz_adg(ch)) {
      parser__increment(ptx);
      len++;
    }
    if (parser_getRegister(tok.val + 1, len - 1) == -1) {
      tok.type = TOK_UNKNOWN;
      ptx->stop = true;
    }
    tok_len(len);
  }

  /* Const token. */
  else if (ch == '#') {
    tok_start(TOK_CONST);
    parser__increment(ptx);
    pos_t len = 1;
    /* Detect the base. */
    int base = 10;
    if (ch == '0') {
      parser__increment(ptx);
      len++;
      if (ch == 'x') {
        parser__increment(ptx);
        len++;
        base = 16;
      }
      else if (ch == 'b') {
        parser__increment(ptx);
        len++;
        base = 2;
      }
      else
        base = 8;
    }
    /* Determine the length of the const. */
    while (1) {
      if (ch != '_') {
        if (base == 2  && (ch < '0' || ch > '1'))
          break;
        if (base == 8  && (ch < '0' || ch > '7'))
          break;
        if (base == 10 && (ch < '0' || ch > '9'))
          break;
        if (base == 16 && !isz_hex(ch))
          break;
      }
      parser__increment(ptx);
      len++;
    }
    if (len == 1) {
      tok.type = TOK_UNKNOWN;
      ptx->stop = true;
    }
    tok_len(len);
  }

  /* Unknown token. */
  else {
    tok_start(TOK_UNKNOWN);
    tok_len(1);
    ptx->stop = true;
  }

  #undef ch
  #undef tok_start
  #undef tok_len

  ptx->toks[ptx->tok_len] = tok;
  return &ptx->toks[ptx->tok_len++];
}


Token *parser_next(Parser *ptx) {
  if (!ptx)
    return NULL;
  if (ptx->stop)
    return &ptx->toks[ptx->tokpos-1];
  if (ptx->tok_len <= ptx->tokpos)
    if (!parser_tokenize(ptx))
      return NULL;
  return &ptx->toks[ptx->tokpos++];
}


void parser_printToken(Parser *ptx, Token *tok, const char *msg, ...) {
  if (!ptx || !tok || !msg)
    return;

  /* Print the msg. */
  fprintf(stderr, "%s:%"VVARS"u:%"VVARS"u: ",
          ptx->file, tok->line, tok->col);
  va_list args;
  va_start(args, msg);
  vfprintf(stderr, msg, args);
  va_end(args);

  /* Find the start of the line. */
  char const *ln = tok->val;
  while (ln > ptx->src && *(ln - 1) != '\n')
    ln--;

  /* Print the line. Presumably, writing to stderr is
     buffered. I doubt if fputc() is efficient here. */
  fprintf(stderr, "%5"VVARS"u | ", tok->line);
  pos_t col = 1;
  while (*ln != '\n' && *ln != '\r' && *ln != '\0') {
    if (*ln == '\t') {
      int tabsz = 8 - (col % 8);
      col += tabsz;
      for (int i = 0; i < tabsz; i++)
        fputc(' ', stderr);
    }
    /* We don't want these whitespace. */
    else if (*ln == '\v' || *ln == '\f') {
      col++;
      fputc(' ', stderr);
    }
    else {
      col++;
      fputc(*ln, stderr);
    }
    ln++;
  }
  fputc('\n', stderr);

  /* Print the markings. */
  fprintf(stderr, "      | ");
  for (pos_t i = 1; i < tok->col; i++)
    fputc(' ', stderr);
  for (pos_t i = 0; i < tok->len; i++)
    fputc('^', stderr);
  fputc('\n', stderr);
}


int parser_getRegister(char *str, int len) {
  #define match(n, v) do { \
      if (strncmp(str, (n), len) == 0) \
        return (v);        \
    } while (0)
  match("r0", 0);
  match("r1", 1);
  match("r2", 2);
  match("r3", 3);
  match("r4", 4);
  match("r5", 5);
  match("r6", 6);
  match("r7", 7);
  match("r8", 8);
  match("r9", 9);
  match("rv", 10);
  match("lr", 11);
  match("bp", 12);
  match("sp", 13);
  match("pc", 14);
  match("fl", 15);
  #undef match
  return -1;
}
