#include <stdio.h>
#include <stdlib.h>
#include "as.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    char* infile = argv[1];
    char* outfile = "out.bin";

    char* buf;
    size_t len;
    if ((len = loadFile(infile, buf)) == 0) return 1;

    free(buf);

    return 0;
}

