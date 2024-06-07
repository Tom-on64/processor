#ifndef EMU_H
#define EMU_H

#include <stdint.h>
#include <stdbool.h>

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

void poke(state_t*, uint16_t, uint8_t);
int load(state_t*, char*, uint16_t);
void step(state_t*);

#endif
