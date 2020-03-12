#ifndef PARSER_H_
#define PARSER_H_

typedef struct AST {
  int unused;
} AST;

int parse_file(const char* file_name);
int parse_string(const char* buff, unsigned long size);

#endif
