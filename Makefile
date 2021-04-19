
# this is picked up by nmake on windows

all: concurrency.exe

concurrency.exe: concurrency.obj thread_helper.obj
	link $(LFLAGS) $** /out:concurrency.exe

clean:
	-del *.obj
	-del concurrency.exe
