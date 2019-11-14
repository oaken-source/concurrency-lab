
CFLAGS ?= -Wall -Wextra -O0 -g

BIN = unguarded turns flags custom

.PHONY: all clean
all: $(BIN)

clean:
	$(RM) $(BIN)

unguarded: concurrency.c
	$(CC) $(CFLAGS) -DHAVE_UNGUARDED -o $@ $< -lpthread

turns: concurrency.c
	$(CC) $(CFLAGS) -DHAVE_TURNS -o $@ $< -lpthread

flags: concurrency.c
	$(CC) $(CFLAGS) -DHAVE_FLAGS -o $@ $< -lpthread

custom: concurrency.c
	$(CC) $(CFLAGS) -DHAVE_CUSTOM -o $@ $< -lpthread
