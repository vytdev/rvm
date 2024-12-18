#ifndef RAS_PARSER_H_
#define RAS_PARSER_H_
#include "../rvm/config.h"
#include <stdbool.h>

typedef uvar pos_t;

typedef enum TokType {
  TOK_UNKNOWN,
  TOK_EOF,
  TOK_LINE,
  TOK_COMMA,
  TOK_TEXT,
  TOK_LABEL,
  TOK_REG,
  TOK_CONST,
} TokType;

typedef struct Token {
  TokType type;
  char *val;
  uvar len;
  pos_t pos;
  pos_t line;
  pos_t col;
} Token;

typedef struct Parser {
  /* Token record. */
  Token *toks;
  pos_t tokpos;
  uvar tok_len;
  uvar tok_alloc;
  /* Current source state. */
  char *file;
  char *src;
  uvar len;
  pos_t pos;
  pos_t line;
  pos_t col;
  bool  stop;
} Parser;


/* whitespace (exc. vertical-tab & form-feed) */
#define isz_wsp(c) \
  ((c) == ' ' || (c) == '\t' || (c) == '\r' || (c) == '\n')
/* digit */
#define isz_dig(c) \
  ((c) >= '0' && (c) <= '9')
/* latin letter */
#define isz_alp(c) \
  (((c) >= 'A' && (c) <= 'Z') || ((c) >= 'a' && (c) <= 'z'))
/* alphanum */
#define isz_adg(c) \
  (isz_dig((c)) || isz_alp((c)))
/* identifier */
#define isz_idn(c) \
  (isz_adg((c)) || (c) == '_' || (c) == '-' || (c) == '.')
/* hex digit */
#define isz_hex(c) \
  (isz_dig((c)) || ((c) >= 'A' && (c) <= 'F') || ((c) >= 'a' && (c) <= 'f'))


/* Initialise a parser context. */
bool parser_init(Parser *ptx, char *src, char *file);

/* Free a parser context. */
void parser_free(Parser *ptx);

/* Generate the next token. */
Token *parser_tokenize(Parser *ptx);

/* Consume the next token. */
Token *parser_next(Parser *ptx);

/* Pretty print a token. */
void parser_printToken(Parser *ptx, Token *tok, const char *msg, ...);

/* Get a register by name. */
int parser_getRegister(char *str, int len);

#endif // RAS_PARSER_H_
