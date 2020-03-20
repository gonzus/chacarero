#include "log.h"
#include "parser.h"

int main(int argc, char *argv[]) {
    int rc = 0;
    for (int j = 1; j < argc; ++j) {
        LOG_INFO("Parsing [%s]", argv[j]);
        rc = parse_file(argv[j]);
    }
    return rc;
}
