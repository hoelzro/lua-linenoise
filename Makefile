OS=$(shell uname)

OBJECTS=linenoise.o encodings/utf8.o linenoiselib.o

ifeq ($(OS),Darwin)
linenoise.dylib: $(OBJECTS)
	gcc -o $@ -bundle -undefined dynamic_lookup $^ $(OPT_LIB)
else
CFLAGS=-fPIC -I/usr/include/lua5.1
linenoise.so: $(OBJECTS)
	gcc -o $@ -shared $^ $(OPT_LIB)
endif

clean:
	rm -f *.o encodings/*.o *.so *.dylib
