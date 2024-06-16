#include <stdio.h>
#include <stdlib.h>
#include "as.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    char* buf;
    FILE* fp; 
    if ((fp = fopen(argv[1], "r")) == NULL) {
        fprintf(stderr, "File not found!\n");
        return 1;
    }

    fseek(fp, 0, SEEK_END);
    size_t len = ftell(fp);
    rewind(fp);
    if ((buf = malloc(len)) == NULL) {
        fprintf(stderr, "Not enough memory!\n");
        return 1;
    }

    if (fread(buf, len, 1, fp) != 1) {
        fprintf(stderr, "Failed while reading file!\n");
        return 1;
    }

    assemble(buf, len);
    
    fclose(fp);
    free(buf);

    return 0;
}

int assemble(char* code, size_t len) {
    printf("Filesize: %lu Bytes\n", len);

    return 0;
}

