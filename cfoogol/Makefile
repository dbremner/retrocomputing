# Makefile for foogol
#

#CC = gcc
CC = clang
CFLAGS	= -O -g -Weverything -std=c99
#CFLAGS	= -O -g -Wall -Wextra -pedantic -Wstrict-prototypes -Wold-style-definition -std=c99

OBJECTS	= \
	foogol.o

all:	foogol

$(OBJECTS) : foogol.h

foogol:	$(OBJECTS)
	$(CC) $(CFLAGS) -o foogol $(OBJECTS)

clean:
	rm *.o foogol
