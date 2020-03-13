#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"
#include "grammar.h"
#include "lexer.h"
#include "parser.h"

#define TRACE_FILE "trace.out"

/* definitions needed to interface with the lemon parser */

typedef void* (MallocFunc)(size_t size);
typedef void (FreeFunc)(void* pointer);

void *ParseAlloc(MallocFunc malloc_func);
void ParseFree(void* lemon, FreeFunc free_func);
void Parse(void *lemon, int token, Value* value, AST *ast);
void ParseTrace(FILE *fp, char *prompt);

int parse_file(const char* file_name) {
    FILE *fp = 0;
    char *buff = 0;
    int rc = 0;

    do {
        /* Open input file */
        fp = fopen(file_name, "r");
        if (fp == NULL) {
            LOG_WARNING("Can't open test file [%s]", file_name);
            break;
        }

        /* Get file size */
        fseek(fp, 0, SEEK_END);
        unsigned long size = ftell(fp);
        rewind(fp);

        /* Allocate buffer and read */
        buff = (char*) malloc(size);
        if (!buff) {
            LOG_WARNING("Cannot allocate %ld bytes", size);
            break;
        }

        /* Slurp whole file */
        unsigned long bytes = fread(buff, 1, size, fp);
        if (bytes != size) {
            LOG_WARNING("Error reading %ld bytes from input file [%s]", size, file_name);
            break;
        }

        rc = parse_string(buff, size);
    } while (0);

    /* CLEANUPS */

    if (buff) {
        free(buff);
        buff = 0;
    }
    if (fp) {
        fclose(fp);
        fp = 0;
    }

    return rc;
}

int parse_string(const char* buff, unsigned long size) {
    FILE *traceFile = 0;
    void *parser = 0;

    do {
        /* Open trace file */
        traceFile = fopen(TRACE_FILE, "w");
        if (traceFile == NULL) {
            LOG_WARNING("Can't open trace file [%s]", TRACE_FILE);
            break;
        }

        /* Initialize lexer */
        Lexer lexer;
        lexer.top = buff;
        lexer.cur = buff;
        lexer.pos = buff;
        lexer.end = buff + size;
        lexer.line = 1;

        /* Initialize the parser state structure */
        AST ast;
        memset(&ast, 0, sizeof(AST));

        /* Pointer to the end of the buffer */

        /* Create parser and set up tracing */
        parser = ParseAlloc(malloc);
        if (!parser) {
            LOG_WARNING("Cannot allocate parser");
            break;
        }
        ParseTrace(traceFile, "parser >> ");

        while (1) {
            int token = scan(&lexer);
            Value* value = value_make(&lexer, token);
            Parse(parser, token, value, &ast);

            // Execute Parse for the last time
            if (token == END_TOKEN) {
                Parse(parser, 0, NULL, &ast);
                break;
            }
        }

        /* TODO do something with AST */
    } while (0);

    /* CLEANUPS */

    if (parser) {
        ParseFree(parser, free);
        parser = 0;
    }
    if (traceFile) {
        fclose(traceFile);
        traceFile = 0;
    }

    return 0;
}
