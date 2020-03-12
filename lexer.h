#ifndef LEXER_H_
#define LEXER_H_

/* definitions needed to interface with the re2c lexer */

typedef struct Lexer {
  const char *top;
  const char *cur;
  const char *ptr;
  const char *pos;
  const char *end;
  int line;
} Lexer;

int scan(Lexer* lexer);

#endif
