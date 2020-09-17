PROGRAM   = zig_zag
CC        = gcc
CFLAGS    = -g -Wall
LDFLAGS   = -lGL -lGLU -lglut -lm

$(PROGRAM): main.o
	$(CC) -o $(PROGRAM) $< $(LDFLAGS)

.PHONY: clean dist

clean:
	-rm *.o $(PROGRAM) 

dist: clean
	-tar -chvj -C .. -f ../$(PROGRAM).tar.bz2 $(PROGRAM)
