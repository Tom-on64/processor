#ifndef EMU_H
#define EMU_H

#include <stdint.h>
#include <stdbool.h>

#define I_NAMES ((const char*[]) {\
        "mov", "ldw", "stw", "push", "pop", "lda", "jnz", "inb",\
        "outb", "add", "adc", "and", "or", "nor", "cmp", "sub" })
#define I_NAME(_i) (I_NAMES)[(uint8_t)(_i >> 4)]

enum Instruction {
    I_MOV   = 0x00,
    I_LDW   = 0x10,
    I_STW   = 0x20,
    I_PUSH  = 0x30,
    I_POP   = 0x40,
    I_LDA   = 0x50,
    I_JNZ   = 0x60,
    I_INB   = 0x70,
    I_OUTB  = 0x80,
    I_ADD   = 0x90,
    I_ADC   = 0xA0,
    I_AND   = 0xB0,
    I_OR    = 0xC0,
    I_NOR   = 0xD0,
    I_CMP   = 0xE0,
    I_SUB   = 0xF0,
};

enum Register {
    R_A = 0,
    R_B = 1,
    R_C = 2,
    R_D = 3,
    R_Z = 4,
    R_L = 5,
    R_H = 6,
    R_F = 7,
};

#define ROM_SIZE 0x8000
#define BANK_SIZE 0x4000
#define RAM_SIZE 0x3ff0

#define MAX_BANKS 256

typedef struct {
    // Status register (0x00)
    uint8_t status;

    // Memory
    union {
        struct {
            uint8_t rom[ROM_SIZE];
            uint8_t bank[BANK_SIZE];
            uint8_t ram[RAM_SIZE];

            uint8_t reserved[11]; // Unused memory mapped register space
            uint8_t mb;  // Memory Bank
            uint16_t sp; // Stack Pointer
            uint16_t pc; // Program Counter
        };
        uint8_t raw[0xffff];
    } memory;

    // If the pointer is NULL, the bank is not present
    void* banks[MAX_BANKS]; 

    // Registers
    union {
        struct { // 8-bit representation
            uint8_t a, b, c, d, z, h, l, f;
        };

        struct { // 16-bit representation
            uint16_t ab, cd;
            uint8_t __ignore0;
            uint16_t hl;
            uint8_t __ignore1;
        };

        uint8_t raw[8];
    } regs;

    // Emulator flags
    bool running;
} state_t;

// Emulator commands
uint8_t peek(state_t*, uint16_t);
void poke(state_t*, uint16_t, uint8_t);
int load(state_t*, char*, uint16_t);
void step(state_t*);

// Processor instructions
uint8_t inb(state_t*, uint8_t);
void outb(state_t*, uint8_t, uint8_t);
void push(state_t*, uint8_t);
uint8_t pop(state_t*);

#endif
