CC=gcc
CFLAGS=-Wall
DEPS = my_tree.c
OBJ = my_tree.o
BIN = my_tree.x

all: $(BIN)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BIN): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(OBJ) $(BIN)