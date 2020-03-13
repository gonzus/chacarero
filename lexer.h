#ifndef LEXER_H_
#define LEXER_H_

/* definitions needed to interface with the re2c lexer */


typedef struct Lexer {
  const char *top; // points to the start of the current token
  const char *cur; // points to the character being examined
  const char *ptr; // used by re2c for back-tracking
  const char *pos; // points to the start of the line
  const char *end; // points one past the end of input
  int line;        // current line in input
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
