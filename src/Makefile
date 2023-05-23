CC=gcc
C++=g++
LD=g++
CFLAGS=-c -Wall -pedantic-errors -O0 -g3 -std=c11 -I../
LDFLAGS=
OBJECTS=main.o ../exp1lib.o
EXECUTABLE=main

all: $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(LD) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(C++) $(CFLAGS) $< -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	-rm -f ${EXECUTABLE} ${EXECUTABLE}.exe ${OBJECTS} core tmp.txt

