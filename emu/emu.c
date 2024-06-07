#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include "emu.h"

int main(int argc, char** argv) {
    state_t state;
    memset(&state, 0, sizeof(state));

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <ROM Image>\n", argv[0]);
        return 1;
    }

    if (load(&state, argv[1], 0) != 0) {
        fprintf(stderr, "%s: ROM Image file not found!\n", argv[0]);
        return 1;
    }

    for (int i = 0; i < 256; i++) {
        if (i % 16 == 0) {
            printf("\n0x%04x: ", i);
        }

        printf("%02x ", state.memory.raw[i]);
    }
    printf("\n");

    while (!(state.status & 0x01)) {
        printf("%04x: %02x\n", state.memory.pc, peek(&state, state.memory.pc));
        step(&state);
        sleep(1);
    }

    return 0;
}

void step(state_t* state) {
    if (state->status & 0x01) return;

    uint8_t ins = peek(state, state->memory.pc++);
    
}

uint8_t peek(state_t* state, uint16_t address) {
    // TODO: memory banks
    return state->memory.raw[address];
}

void poke(state_t* state, uint16_t address, uint8_t value) {
    // TODO: Add memory bank stuff and write protection in ROM area
    state->memory.raw[address] = value;
}

int load(state_t* state, char* file, uint16_t address) {
    FILE* fp = fopen(file, "rb");
    
    if (fp == NULL) return -1;

    fseek(fp, 0, SEEK_END);
    size_t len = ftell(fp);
    rewind(fp);

    if (address + len > 0xffff) {
        fprintf(stderr, "File too large! Will overwrite %lu bytes starting at 0x0000.\n", (address + len) % 0xffff);
    }

    uint8_t* buf = malloc(len);
    if (!buf || fread(buf, len, 1, fp) != 1) {
        return -1;
    }

    size_t i = 0;
    while (i != len) { // Poke data into memory
        poke(state, address + i, ((uint8_t*)buf)[i]);
        i++;
    }

    fclose(fp);
    free(buf);
    return 0;
}

