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
        printf("%04x: %s\n", state.memory.pc, I_NAME(peek(&state, state.memory.pc)));
        step(&state);
        sleep(1);
    }

    printf("\nFinal state:\na: 0x%x b: 0x%x c: 0x%x d: 0x%x z: 0x%x hl: 0x%x\n", state.regs.a, state.regs.b, state.regs.c, state.regs.d, state.regs.z, state.regs.hl);

    return 0;
}

void step(state_t* state) {
    if (state->status & 0x01) return;

    uint8_t ins = peek(state, state->memory.pc++);
    uint8_t im8;
    uint16_t im16;

    switch (ins & 0xf0) {
        case I_MOV:     // reg, reg/im8
            im8 = peek(state, state->memory.pc++);
            if (ins & 0x08) state->regs.raw[ins & 0x07] = im8;
            else state->regs.raw[ins & 0x07] = state->regs.raw[im8 & 0x07];
            break;
        case I_LDW:     // reg, [HL/im16]
            if (ins & 0x08) {
                im16 = (uint16_t)peek(state, state->memory.pc++) | (uint16_t)(peek(state, state->memory.pc++) << 8);
                state->regs.raw[ins & 0x07] = peek(state, im16);
            } else {
                state->regs.raw[ins & 0x07] = peek(state, state->regs.hl);
            }
            break;
        case I_STW:     // [HL/im16], reg
            if (ins & 0x08) {
                im16 = (uint16_t)peek(state, state->memory.pc++) | (uint16_t)(peek(state, state->memory.pc++) << 8);
                poke(state, im16, state->regs.raw[ins & 0x07]);
            } else {
                poke(state, state->regs.hl, state->regs.raw[ins & 0x07]);
            }
            break;
        case I_PUSH:    // reg/im8
        case I_POP:     // reg
        case I_LDA:     // im16
        case I_JNZ:     // reg/im8
        case I_INB:     // reg, reg/im8
            im8 = peek(state, state->memory.pc++);
            if (ins & 0x08) state->regs.raw[ins & 0x07] = inb(state, im8);
            else state->regs.raw[ins & 0x07] = inb(state, state->regs.raw[im8 & 0x07]);
            break;
        case I_OUTB:    // reg, reg/im8
            im8 = peek(state, state->memory.pc++);
            if (ins & 0x08) outb(state, im8, state->regs.raw[ins & 0x07]);
            else outb(state, state->regs.raw[im8 & 0x07], state->regs.raw[ins & 0x07]);
            break;
        case I_ADD:     // reg, reg/im8
            im8 = peek(state, state->memory.pc++);
            if (ins & 0x08) state->regs.raw[ins & 0x07] += im8;
            else state->regs.raw[ins & 0x07] += state->regs.raw[im8 & 0x07];
            break;
        case I_ADC:     // reg, reg/im8
        case I_AND:     // reg, reg/im8
        case I_OR:      // reg, reg/im8
            im8 = peek(state, state->memory.pc++);
            if (ins & 0x08) state->regs.raw[ins & 0x07] |= im8;
            else state->regs.raw[ins & 0x07] |= state->regs.raw[im8 & 0x07];
            break;
        case I_NOR:     // reg, reg/im8
        case I_CMP:     // reg, reg/im8
        case I_SUB:     // reg, reg/im8
        default:
            break;
    }
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

uint8_t inb(state_t* state, uint8_t port) {
    if (port == 0x00) {
        return state->status;
    }
    return 0;
}

void outb(state_t* state, uint8_t port, uint8_t value) {
    if (port == 0x00) {
        state->status = value;
    }
}

