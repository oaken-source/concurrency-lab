
CFLAGS ?= -Wall -Wextra -O0 -g
CFLAGS += -pthread

BIN = unguarded turns flags peterson dekker bakery test_and_set semaphore custom
SRC = concurrency.c thread_helper.c

.PHONY: all clean
all: $(BIN)

clean:
	$(RM) $(BIN)

unguarded: $(SRC)
	$(CC) $(CFLAGS) -DHAVE_UNGUARDED -o $@ $^

turns: $(SRC)
	$(CC) $(CFLAGS) -DHAVE_TURNS -o $@ $^

flags: $(SRC)
	$(CC) $(CFLAGS) -DHAVE_FLAGS -o $@ $^

peterson: $(SRC)
	$(CC) $(CFLAGS) -DHAVE_PETERSON -o $@ $^

dekker: $(SRC)
	$(CC) $(CFLAGS) -DHAVE_DEKKER -o $@ $^

bakery: $(SRC)
	$(CC) $(CFLAGS) -DHAVE_BAKERY -o $@ $^

test_and_set: $(SRC)
	$(CC) $(CFLAGS) -DHAVE_TEST_AND_SET -o $@ $^

semaphore: $(SRC)
	$(CC) $(CFLAGS) -DHAVE_SEMAPHORE -o $@ $^

custom: $(SRC)
	$(CC) $(CFLAGS) -DHAVE_CUSTOM -o $@ $^