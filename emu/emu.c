#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "emu.h"

#define VERSION "1.0.0"

#define MAX_INPUT 1024
#define MAX_OUTPUT 1024
#define PROMPT "(emu) "

void command(state_t*, char*, char*);

int main(int argc, char** argv) {
    printf("\nEMU %s monitor - Type 'help' for more information\n\n", VERSION);

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

    /*
    for (int i = 0; i < 256; i++) {
        if (i % 16 == 0) {
            printf("\n0x%04x: ", i);
        }

        printf("%02x ", state.memory.raw[i]);
    }
    printf("\n");
    */

    while (1) {
        char buf[MAX_INPUT] = { 0 };
        char out[MAX_OUTPUT] = { 0 };

        printf(PROMPT);
        if (fgets(buf, MAX_INPUT, stdin) == NULL) break;
        size_t len = strlen(buf);

        if (len == 0) continue;
        if (len > 0 && buf[len - 1] == '\n') {
            buf[len - 1] = '\0';
        }

        command(&state, buf, out);

        if (strlen(out) != 0) fprintf(stdout, "%s", out);
    }

    return 0;
}

void command(state_t* state, char* command, char* out) {
    char str[256];
    char* token;
    char* tokens[4];
    uint8_t tokenCount = 0;

    strlcpy(str, command, sizeof(str));

    token = strtok(str, " ");
    while (token != NULL) {
        if (tokenCount > 4) {
            strcpy(out, "Too many arguments given\n");
            return;
        }

        tokens[tokenCount++] = token;
        token = strtok(NULL, " ");
    }

    if (tokenCount == 0) {
        return;
    } else if (strcmp(tokens[0], "exit") == 0) {
        exit(0);
    } else if (strcmp(tokens[0], "goto") == 0) {
        if (tokenCount < 2) {
            strcpy(out, "Expected argument!\n");
            return;
        }

        uint16_t addr = (uint16_t)strtoul(tokens[1], NULL, 0);
        state->memory.pc = addr;
        sprintf(out, "Jumped to 0x%04x\n", state->memory.pc);
    } else if (strcmp(tokens[0], "halt") == 0) {
        uint8_t status = state->status;
        if (status & 0x01) strcpy(out, "System unhalted\n");
        else strcpy(out, "System halted\n");
        state->status = status ^ 0x01;
    } else if (strcmp(tokens[0], "help") == 0) {
        strcpy(out, "EMU " VERSION " monitor help:\n"
                " exit              Stops the emulator and exits\n"
                " goto <addr>       Sets PC to address\n"
                " halt              Toggles the halt bit\n"
                " help              Prints this help message\n"
                " peek <addr>       Prints byte at memory address\n"
                " poke <addr> <val> Sets a byte at memory address\n"
                //" reboot            Reboots the system\n"
                " regs              Prints register values\n"
                " run               Runs until halt\n"
                " step              Runs a single clock cycle\n"
              );
    } else if (strcmp(tokens[0], "peek") == 0) {
        if (tokenCount < 2) {
            strcpy(out, "Expected argument!\n");
            return;
        }

        uint16_t addr = (uint16_t)strtoul(tokens[1], NULL, 0);
        uint8_t value = peek(state, addr);
        sprintf(out, "0x%04x: 0x%02x %4s\n", addr, value, I_NAME(value));
    } else if (strcmp(tokens[0], "poke") == 0) {
        if (tokenCount < 3) {
            strcpy(out, "Expected argument!\n");
            return;
        }

        uint16_t addr = (uint16_t)strtoul(tokens[1], NULL, 0);
        uint8_t value = (uint8_t)strtoul(tokens[2], NULL, 0);
        poke(state, addr, value);
        sprintf(out, "[0x%04x] = 0x%02x\n", addr, value);
    //} else if (strcmp(tokens[0], "reboot") == 0) {
    } else if (strcmp(tokens[0], "regs") == 0) {
        sprintf(out, "Register Values:\n"
                    "A: 0x%02x\n"
                    "B: 0x%02x\n"
                    "C: 0x%02x\n"
                    "D: 0x%02x\n"
                    "Z: 0x%02x\n"
                    "HL: 0x%04x\n"
                    "F: 0x%02x\n"
                    "MB: 0x%02x\n"
                    "SP: 0x%04x\n"
                    "PC: 0x%04x\n",
                    state->regs.a, state->regs.b, state->regs.c, state->regs.d, state->regs.z, state->regs.hl, state->regs.f,
                    state->memory.mb, state->memory.sp, state->memory.pc
               );
    } else if (strcmp(tokens[0], "run") == 0) {
        while (!(state->status & 0x01)) {
            step(state);
        }
        sprintf(out, "System halted at 0x%04x\n", state->memory.pc);
    } else if (strcmp(tokens[0], "step") == 0) {
        uint8_t opcode = peek(state, state->memory.pc);
        sprintf(out, "0x%04x: %02x %4s\n", state->memory.pc, opcode, I_NAME(opcode));
        step(state);
    } else {
        sprintf(out, "Unknown command '%s' Use 'help' for a list of available commands\n", tokens[0]);
    }
}

