#include <stdlib.h>
#include <string.h>
#include "gmem.h"
#include "grammar.h"
#include "lexer.h"

Value* value_make(Lexer* lexer, int token) {
    Value* value = 0;
    int rc = 0;
    int len = lexer->cur - lexer->top;
    switch (token) {
        case INT_LITERAL:
            GMEM_NEW(value, Value*, sizeof(Value));
            value->kind = ValueInt;
            value->vint = 0;
            for (int j = 0; j < len; ++j) {
                const char* p = lexer->top + j;
                value->vint = value->vint * 10 + *p - '0';
            }
            break;
        case NAME:
            GMEM_NEW(value, Value*, sizeof(Value));
            value->kind = ValueSymbol;
            GMEM_NEWSTR(value->vsym, lexer->top, len, rc);
            (void) rc;
            break;
        case STRING_LITERAL_SINGLE:
        case STRING_LITERAL_DOUBLE:
            GMEM_NEW(value, Value*, sizeof(Value));
            value->kind = ValueString;
            len -= 2;
            GMEM_NEWSTR(value->vstr, lexer->top + 1, len, rc);
            (void) rc;
            break;
    }
    return value;
}

void value_destroy(Value* value) {
    if (!value) return;
    switch (value->kind) {
        case ValueInt:
            GMEM_DEL(value, Value*, sizeof(Value));
            break;
        case ValueDouble:
            GMEM_DEL(value, Value*, sizeof(Value));
            break;
        case ValueString:
            GMEM_DELSTR(value->vstr, 0);
            GMEM_DEL(value, Value*, sizeof(Value));
            break;
        case ValueSymbol:
            GMEM_DELSTR(value->vsym, 0);
            GMEM_DEL(value, Value*, sizeof(Value));
            break;
    }
}
