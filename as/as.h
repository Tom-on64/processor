#ifndef AS_H
#define AS_H

#include <stdint.h>
#include <stdbool.h>
#include "../common/proc.h"

enum TokenType {
    END_OF_FILE,
};

typedef struct {
    enum Instruction type;
    bool Immediate; // For instructions that have HL/im16 HL is implied if this is false
    union {
        enum Register reg;
        uint8_t im8;
        uint16_t im16;
    } value;
} ins_t;

typedef struct {
    enum TokenType type;
    // TODO: Content
    uint32_t row;
    uint32_t col;
} token_t;

size_t loadFile(char*, void*);
uint32_t tokenize(char*, size_t, token_t*);

#endif
