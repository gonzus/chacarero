#include <stdlib.h>
#include <string.h>
#include "mem.h"
#include "grammar.h"
#include "lexer.h"

#define ACCUM(v, p) v = ((v) * 10 + *(p) - '0')

Value* value_make(Lexer* lexer, int token) {
    Value* value = 0;
    int len = lexer->cur - lexer->top;
    switch (token) {
        case LIT_INT: {
            long v = 0;
            MEM_MALLOC(value, Value*, sizeof(Value));
            value->kind = ValueInt;
            for (int j = 0; j < len; ++j) {
                const char* p = lexer->top + j;
                ACCUM(v, p);
            }
            value->vlng = v;
            break;
        }

        case LIT_DBL: {
            long v[2] = {0, 0};
            long o = 1;
            int s = 0;
            MEM_MALLOC(value, Value*, sizeof(Value));
            value->kind = ValueDouble;
            for (int j = 0; j < len; ++j) {
                const char* p = lexer->top + j;
                if (*p == '.') {
                    s = 1;
                    continue;
                }
                ACCUM(v[s], p);
                if (s) {
                    o *= 10;
                }
            }
            value->vdbl = (double) v[0] + (double) v[1] / (double) o;
            break;
        }

        case NAME:
            MEM_MALLOC(value, Value*, sizeof(Value));
            value->kind = ValueSymbol;
            MEM_STR_DUP(value->vsym, lexer->top, len);
            break;

        case LIT_STS:
        case LIT_STD:
            MEM_MALLOC(value, Value*, sizeof(Value));
            value->kind = ValueString;
            len -= 2;
            MEM_STR_DUP(value->vstr, lexer->top + 1, len);
            break;
    }
    return value;
}

void value_destroy(Value* value) {
    if (!value) return;
    switch (value->kind) {
        case ValueInt:
            MEM_FREE(value, Value*, sizeof(Value));
            break;
        case ValueDouble:
            MEM_FREE(value, Value*, sizeof(Value));
            break;
        case ValueString:
            MEM_STR_FREE(value->vstr, 0);
            MEM_FREE(value, Value*, sizeof(Value));
            break;
        case ValueSymbol:
            MEM_STR_FREE(value->vsym, 0);
            MEM_FREE(value, Value*, sizeof(Value));
            break;
    }
}
