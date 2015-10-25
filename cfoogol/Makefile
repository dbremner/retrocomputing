# Makefile for foogol
#

CC = gcc
CFLAGS	= -O -g -Wall -Wextra -pedantic -Wstrict-prototypes -Wold-style-definition -std=c99

OBJECTS	= \
	foogol.o

all:	foogol

$(OBJECTS) : foogol.h

foogol:	$(OBJECTS)
	$(CC) $(CFLAGS) -o foogol $(OBJECTS)

clean:
	rm *.o foogol
