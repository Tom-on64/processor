CC = clang
LD = clang

CFLAGS = -std=c11 -O2 -g -Wall -Wextra -Wpedantic\
		 -Wno-pointer-arith -Wno-unused-parameter
LDFLAGS = 

BIN_DIR = ./bin

EMU_SRC = $(wildcard emu/*.c)
EMU_OBJ = $(EMU_SRC:.c=.o)

EMU = $(BIN_DIR)/emu

all: emu

clean:
	rm -rf $(BIN_DIR)
	rm -f ./**/*.o
	
%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

dirs:
	[[ -d $(BIN_DIR) ]] || mkdir $(BIN_DIR)

emu: dirs $(EMU_OBJ)
	$(LD) -o $(EMU) $(filter %.o, $^) $(LDFLAGS)

