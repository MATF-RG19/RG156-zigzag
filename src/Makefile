PROGRAM   = zig_zag
CC        = gcc
CFLAGS    =  -g -ansi -Wall -I/usr/X11R6/include -I/usr/pkg/include
LDFLAGS   =  -L/usr/X11R6/lib -L/usr/pkg/lib
LDLIBS  = -lglut -lGLU -lGL -lm

$(PROGRAM): main.c  lopta.c  image.c bitmap_functions.c
	$(CC) $(LDFLAGS) -o $(PROGRAM) $^ $(LDLIBS)

.PHONY: clean dist

clean:
	-rm  $(PROGRAM) 

dist: clean
	-tar -chvj -C .. -f ../$(PROGRAM).tar.bz2 $(PROGRAM)
