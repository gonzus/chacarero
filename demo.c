#include "parser.h"

#define INPUT_FILE "test.dat"

int main(int argc, char *argv[]) {
    int rc = parse_file(INPUT_FILE);
    return rc;
}
