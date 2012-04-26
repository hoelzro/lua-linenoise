CFLAGS=-fPIC -I/usr/include/lua5.1

linenoise.so: linenoise.o linenoiselib.o
	gcc -o linenoise.so -shared linenoise.o linenoiselib.o

clean:
	rm -f *.o *.so
