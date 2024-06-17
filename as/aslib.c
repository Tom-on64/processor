#include <stdio.h>
#include <stdlib.h>
#include "as.h"

size_t loadFile(char* file, void* buf) {
    FILE* fp; 
    if ((fp = fopen(file, "r")) == NULL) {
        fprintf(stderr, "File not found!\n");
        return 0;
    }

    fseek(fp, 0, SEEK_END);
    size_t len = ftell(fp);
    rewind(fp);
    if ((buf = malloc(len)) == NULL) {
        fprintf(stderr, "Not enough memory!\n");
        return 0;
    }

    if (fread(buf, len, 1, fp) != 1) {
        fprintf(stderr, "Failed while reading file!\n");
        return 0;
    }

    fclose(fp);
    return len;
}

