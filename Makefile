CFLAGS=-fPIC -I/usr/include/lua5.1 -Ilib

linenoise.so: linenoise.o lib/linenoise.o
	gcc -o linenoise.so -shared linenoise.o lib/linenoise.o

clean:
	rm -f *.o *.so
