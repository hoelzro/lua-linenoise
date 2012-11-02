OS=$(shell uname)

ifeq ($(LIBLINENOISE),)
MY_LIBLINENOISE=linenoiselib.o
endif

ifeq ($(OS),Darwin)
linenoise.dylib: linenoise.o $(MY_LIBLINENOISE)
	gcc -o $@ -bundle -undefined dynamic_lookup $^ $(LIBLINENOISE)
else
CFLAGS=-fPIC -I/usr/include/lua5.1
linenoise.so: linenoise.o $(MY_LIBLINENOISE)
	gcc -o $@ -shared $^ $(LIBLINENOISE)
endif

clean:
	rm -f *.o *.so *.dylib
