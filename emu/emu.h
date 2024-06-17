#ifndef EMU_H
#define EMU_H

#include <stdint.h>
#include <stdbool.h>
#include "../common/proc.h"

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
