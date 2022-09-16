CFLAGS = -ansi -g -Wall -Werror -Wextra -Wformat=2 -Wjump-misses-init 
CFLAGS += -Wlogical-op -Wpedantic -Wshadow

PROG = my_ls

SRC = ls.c helpers.c
BIN = bin

all: $(PROG)

$(PROG): $(SRC)
	mkdir -p $(BIN)
	$(CC) $(CFLAGS) -o $(BIN)/$(PROG) $(SRC)

clean:
	rm -rf $(PROG)
