OS=$(shell uname)
ifeq ($(OS),Darwin)
linenoise.dylib: linenoise.o linenoiselib.o
	gcc -o $@ -bundle -undefined dynamic_lookup $^
else
CFLAGS=-fPIC -I/usr/include/lua5.1
linenoise.so: linenoise.o linenoiselib.o
	gcc -o $@ -shared $^
endif

clean:
	rm -f *.o *.so *.dylib
