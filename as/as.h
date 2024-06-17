#ifndef AS_H
#define AS_H

#include <stdint.h>
#include "../common/proc.h"

typedef struct {
    enum Instruction type;
    union {
        struct {
            uint8_t reg;
            uint8_t __ignore0;
        };

        struct {
            uint8_t im8;
            uint8_t __ignore1;
        };

        uint16_t im16;
    } value;
} ins_t;

size_t loadFile(char*, void*);
uint32_t tokenize(char*, size_t, token_t*);

#endif
