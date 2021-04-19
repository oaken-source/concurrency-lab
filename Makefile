
# this is picked up by nmake on windows

BIN = unguarded.exe turns.exe flags.exe peterson.exe dekker.exe bakery.exe test_and_set.exe semaphore.exe custom.exe
SRC = concurrency.c thread_helper.c

all: $(BIN)

clean: 
	-del $(BIN)

unguarded.exe: $(SRC)
	cl.exe /DHAVE_UNGUARDED $** /Feunguarded.exe

turns.exe: $(SRC)
	cl.exe /DHAVE_TURNS $** /Feturns.exe

flags.exe: $(SRC)
	cl.exe /DHAVE_FLAGS $** /Feflags.exe

peterson.exe: $(SRC)
	cl.exe /DHAVE_PETERSON $** /Fepeterson.exe

dekker.exe: $(SRC)
	cl.exe /DHAVE_DEKKER $** /Fedekker.exe

bakery.exe: $(SRC)
	cl.exe /DHAVE_BAKERY $** /Febakery.exe

test_and_set.exe: $(SRC)
	cl.exe /DHAVE_TEST_AND_SET $** /Fetest_and_set.exe

semaphore.exe: $(SRC)
	cl.exe /DHAVE_SEMAPHORE $** /Fesemaphore.exe

custom.exe: $(SRC)
	cl.exe /DHAVE_CUSTOM $** /Fecustom.exe
