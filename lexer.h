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

enum ValueKind { ValueSymbol, ValueInt, ValueDouble, ValueString };

typedef struct Value {
    enum ValueKind kind;
    union {
        int vint;
        double vdbl;
        char* vstr;
        char* vsym;
    };
} Value;

int scan(Lexer* lexer);
Value* value_make(Lexer* lexer, int token);
void value_destroy(Value* value);

#endif
