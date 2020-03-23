#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
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
    int fd = -1;
    int size = 0;
    char *buff = 0;
    int rc = 0;

    do {
        fd = open(file_name, O_RDONLY);
        if (fd < 0) {
            LOG_WARNING("Cannot open [%s]", file_name);
            break;
        }
        LOG_DEBUG("Opened file [%s] as descriptor %d", file_name, fd);

        struct stat st;
        int status = fstat(fd, &st);
        if (status < 0) {
            LOG_WARNING("Cannot stat [%s]", file_name);
            break;
        }
        size = st.st_size;
        LOG_DEBUG("Stated file [%s] size %d", file_name, size);

        buff = (char*) mmap (0, size, PROT_READ, MAP_PRIVATE, fd, 0);
        if (buff == MAP_FAILED) {
            LOG_WARNING("Cannot mmap [%s]", file_name);
            buff = 0;
            break;
        }
        LOG_DEBUG("Mapped file [%s] size %d at %p", file_name, size, buff);

        // close file ASAP -- mmap doesn't mind
        rc = close(fd);
        if (rc < 0) {
            LOG_WARNING("Cannot close [%s]", file_name);
        }
        LOG_DEBUG("Closed file [%s] as descriptor %d", file_name, fd);
        fd = -1;

        rc = parse_string(buff, size);
    } while (0);

    if (buff) {
        int rc = munmap(buff, size);
        if (rc < 0) {
            LOG_WARNING("Cannot unmap [%s] from %p", file_name, buff);
        }
        LOG_DEBUG("Unmapped file [%s] size %d from %p", file_name, size, buff);
        buff = 0;
    }
    if (fd >= 0) {
        int rc = close(fd);
        if (rc < 0) {
            LOG_WARNING("Cannot close [%s]", file_name);
        }
        LOG_DEBUG("Closed file [%s] as descriptor %d", file_name, fd);
        fd = -1;
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
