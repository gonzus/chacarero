#include <ctype.h>
#include <stdio.h>
#include "log.h"
#include "grammar.h"
#include "lexer.h"

// these are required by the re2c generated code
#define   YYCTYPE     char         // data type of each input symbol
#define   YYCURSOR    lexer->cur   // current cursor location
#define   YYMARKER    lexer->ptr   // used by re2c to store the position for back-tracking

// this check probably should at the beginning of every lexer "mode" (regular, comment, ...)
#define LEXER_CHECK_EOF  do { if (lexer->cur >= lexer->end) { return END_TOKEN; } } while (0)

static void show_lexer_error(char c);

int scan(Lexer* lexer) {

regular:
    LEXER_CHECK_EOF;
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
        show_lexer_error(*lexer->top);
        goto regular;
    }
*/

comment:
    LEXER_CHECK_EOF;
    lexer->top = lexer->cur;

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
    char str[32];
    str[0] = '\0';
    if (isprint(c)) {
        sprintf(str, " - %c", c);
    }
    LOG_INFO("LEXER ERROR: unexpected character code 0x%02x - %3d%s", x, x, str);
}
