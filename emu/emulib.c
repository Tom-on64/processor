#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include "emu.h"

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
            if (ins & 0x08) {
                im8 = peek(state, state->memory.pc++);
                push(state, im8);
            } else push(state, state->regs.raw[ins & 0x07]);
            break; 
        case I_POP:     // reg
            state->regs.raw[ins & 0x07] = pop(state);
            break;
        case I_LDA:     // im16
            im16 = (uint16_t)peek(state, state->memory.pc++) | (uint16_t)(peek(state, state->memory.pc++) << 8);
            state->regs.hl = im16;
            break;
        case I_JNZ:     // reg/im8
            if (ins & 0x08) {
                im8 = peek(state, state->memory.pc++);
            } else im8 = state->regs.raw[ins & 0x07];
            if (im8 != 0) {
                state->memory.pc = state->regs.hl;
            }
            break;
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
            im8 = peek(state, state->memory.pc++);
            if (ins & 0x08) state->regs.raw[ins & 0x07] += im8 + (state->regs.f & F_CARRY);
            else state->regs.raw[ins & 0x07] += state->regs.raw[im8 & 0x07] + (state->regs.f & F_CARRY);
            break;
        case I_AND:     // reg, reg/im8
            im8 = peek(state, state->memory.pc++);
            if (ins & 0x08) state->regs.raw[ins & 0x07] &= im8;
            else state->regs.raw[ins & 0x07] &= state->regs.raw[im8 & 0x07];
            break;
        case I_OR:      // reg, reg/im8
            im8 = peek(state, state->memory.pc++);
            if (ins & 0x08) state->regs.raw[ins & 0x07] |= im8;
            else state->regs.raw[ins & 0x07] |= state->regs.raw[im8 & 0x07];
            break;
        case I_NOR:     // reg, reg/im8
            im8 = peek(state, state->memory.pc++);
            if (ins & 0x08) state->regs.raw[ins & 0x07] = ~(state->regs.raw[ins & 0x07] | im8);
            else state->regs.raw[ins & 0x07] = ~(state->regs.raw[ins & 0x07] | state->regs.raw[im8 & 0x07]);
            break;
        case I_CMP:     // reg, reg/im8
            // TODO: Update flags
            break;
        case I_SUB:     // reg, reg/im8
            im8 = peek(state, state->memory.pc++);
            if (ins & 0x08) state->regs.raw[ins & 0x07] -= im8 - (state->regs.f & F_BORROW);
            else state->regs.raw[ins & 0x07] -= state->regs.raw[im8 & 0x07] - (state->regs.f & F_BORROW);
            break;
        default:
            fprintf(stderr, "%04x: %02x - Invalid Opcode\n", state->memory.pc - 1, ins);
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

void push(state_t* state, uint8_t value) {
    poke(state, state->memory.sp--, value);
}

uint8_t pop(state_t* state) {
    return peek(state, ++state->memory.sp);
}


