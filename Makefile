CC = clang
LD = clang

CFLAGS = -std=c11 -O2 -g -Wall -Wextra -Wpedantic\
		 -Wno-pointer-arith -Wno-unused-parameter
LDFLAGS = 

BIN_DIR = ./bin

EMU_SRC = $(wildcard emu/*.c)
EMU_OBJ = $(EMU_SRC:.c=.o)

AS_SRC = $(wildcard as/*.c)
AS_OBJ = $(AS_SRC:.c=.o)

EMU = $(BIN_DIR)/emu
AS  = $(BIN_DIR)/as

all: emu as

clean:
	rm -rf $(BIN_DIR)
	rm -f ./**/*.o
	
%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

dirs:
	[[ -d $(BIN_DIR) ]] || mkdir $(BIN_DIR)

emu: dirs $(EMU_OBJ)
	$(LD) -o $(EMU) $(filter %.o, $^) $(LDFLAGS)

as: dirs $(AS_OBJ)
	$(LD) -o $(AS) $(filter %.o, $^) $(LDFLAGS)

