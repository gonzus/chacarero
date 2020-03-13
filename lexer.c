#include <stdlib.h>
#include <string.h>
#include "gmem.h"
#include "grammar.h"
#include "lexer.h"

#define ACCUM(v, p) v = ((v) * 10 + *(p) - '0')

Value* value_make(Lexer* lexer, int token) {
    Value* value = 0;
    int len = lexer->cur - lexer->top;
    switch (token) {
        case LIT_INT:
            GMEM_MALLOC(value, Value*, sizeof(Value));
            value->kind = ValueInt;
            value->vlng = 0;
            for (int j = 0; j < len; ++j) {
                const char* p = lexer->top + j;
                ACCUM(value->vlng, p);
            }
            break;
        case LIT_DBL: {
            GMEM_MALLOC(value, Value*, sizeof(Value));
            value->kind = ValueDouble;
            value->vdbl = 0;
            int man = 0;
            int dec = 0;
            int off = 1;
            for (int j = 0; j < len; ++j) {
                const char* p = lexer->top + j;
                if (*p == '.') {
                    man = 1;
                    continue;
                }
                if (man) {
                    ACCUM(dec, p);
                    off *= 10;
                } else {
                    ACCUM(value->vdbl, p);
                }
            }
            value->vdbl += (double) dec / (double) off;
            break;
        }
        case NAME:
            GMEM_MALLOC(value, Value*, sizeof(Value));
            value->kind = ValueSymbol;
            GMEM_STR_DUP(value->vsym, lexer->top, len);
            break;
        case LIT_STS:
        case LIT_STD:
            GMEM_MALLOC(value, Value*, sizeof(Value));
            value->kind = ValueString;
            len -= 2;
            GMEM_STR_DUP(value->vstr, lexer->top + 1, len);
            break;
    }
    return value;
}

void value_destroy(Value* value) {
    if (!value) return;
    switch (value->kind) {
        case ValueInt:
            GMEM_FREE(value, Value*, sizeof(Value));
            break;
        case ValueDouble:
            GMEM_FREE(value, Value*, sizeof(Value));
            break;
        case ValueString:
            GMEM_STR_FREE(value->vstr, 0);
            GMEM_FREE(value, Value*, sizeof(Value));
            break;
        case ValueSymbol:
            GMEM_STR_FREE(value->vsym, 0);
            GMEM_FREE(value, Value*, sizeof(Value));
            break;
    }
}
