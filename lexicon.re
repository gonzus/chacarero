#include <ctype.h>
#include <stdio.h>
#include "grammar.h"
#include "lexer.h"

#define   YYCTYPE     char
#define   YYCURSOR    lexer->cur
#define   YYMARKER    lexer->ptr

static void show_lexer_error(char c);

int scan(Lexer* lexer) {

regular:
    if (lexer->cur >= lexer->end) {
        return END_TOKEN;
    }
    lexer->top = lexer->cur;

/*!re2c
    re2c:yyfill:enable = 0;

    whitespace = [ \t\v\f]+;
    dig        = [0-9];
    let        = [a-zA-Z_];
    hex        = [a-fA-F0-9];
    any        = [\000-\377];
*/

/*!re2c

    "#"                 { goto comment; }
    "="                 { return EQ; }
    '('                 { return LPAREN; }
    ")"                 { return RPAREN; }
    "["                 { return LBRACKET; }
    "]"                 { return RBRACKET; }
    "{"                 { return LBRACE; }
    "}"                 { return RBRACE; }
    ","                 { return COMMA; }
    ";"                 { return SEMICOLON; }
    "."                 { return DOT; }
    "->"                { return ARROW; }
    "?"                 { return QUESTION; }
    ":"                 { return COLON; }

    "!"                 { return LNOT; }
    "&&"                { return LAND; }
    "||"                { return LOR; }
    "=="                { return LEQ; }
    "!="                { return LNE; }
    "<"                 { return LLT; }
    ">"                 { return LGT; }
    "<="                { return LLE; }
    ">="                { return LGE; }

    "+"                 { return ADD; }
    "-"                 { return SUB; }
    "*"                 { return MUL; }
    "/"                 { return DIV; }
    "%"                 { return MOD; }

    "~"                 { return BNOT; }
    "&"                 { return BAND; }
    "|"                 { return BOR; }
    "^"                 { return BXOR; }

    "++"                { return ADDADD; }
    "--"                { return SUBSUB; }

    ["]([^"\x00]+)["]   { return STRING_LITERAL_DOUBLE; }
    [']([^'\x00]+)[']   { return STRING_LITERAL_SINGLE; }
    let (let|dig)*      { return NAME; }
    whitespace          { goto regular; }

    ("0" [xX] hex+) | ("0" dig+) | (dig+) {
        return INT_LITERAL;
    }

    "\r\n"|"\n" {
        lexer->pos = lexer->cur;
        lexer->line++;
        goto regular;
    }

    any {
        show_lexer_error(*lexer->cur);
        goto regular;
    }
*/

    comment:
/*!re2c

    "\r\n"|"\n" {
        lexer->pos = lexer->cur;
        lexer->line++;
        goto regular;
    }
    any                 { goto comment; }
*/
}

static void show_lexer_error(char c) {
    unsigned int x = (unsigned int) c;
    fprintf(stderr, "LEXER ERROR: unexpected character code 0x%02x - %3d", x, x);
    if (isprint(c)) {
        fprintf(stderr, " - [%c]", c);
    }
    fprintf(stderr, "\n");
}
