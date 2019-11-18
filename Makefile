
CFLAGS ?= -Wall -Wextra -O0 -g

BIN = unguarded turns flags peterson dekker bakery test_and_set semaphore custom

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

peterson: concurrency.c
	$(CC) $(CFLAGS) -DHAVE_PETERSON -o $@ $< -lpthread

dekker: concurrency.c
	$(CC) $(CFLAGS) -DHAVE_DEKKER -o $@ $< -lpthread

bakery: concurrency.c
	$(CC) $(CFLAGS) -DHAVE_BAKERY -o $@ $< -lpthread

test_and_set: concurrency.c
	$(CC) $(CFLAGS) -DHAVE_TEST_AND_SET -o $@ $< -lpthread

semaphore: concurrency.c
	$(CC) $(CFLAGS) -DHAVE_SEMAPHORE -o $@ $< -lpthread

custom: concurrency.c
	$(CC) $(CFLAGS) -DHAVE_CUSTOM -o $@ $< -lpthread
