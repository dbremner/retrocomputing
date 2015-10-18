# Makefile for the JOSS implementation.

CC = g++ 
CFLAGS = -Wall -Wextra -std=c++14

joss : joss.o parse.o eval.o josssubs.o Makefile
	$(CC) $(CFLAGS) -o joss joss.o parse.o eval.o josssubs.o -lm

joss.o : joss.cpp joss.hpp Makefile
	$(CC) $(CFLAGS) -c joss.cpp -o joss.o

parse.o : parse.cpp joss.hpp Makefile
	$(CC) $(CFLAGS) -c parse.cpp -o parse.o

eval.o : eval.cpp joss.hpp Makefile
	$(CC) $(CFLAGS) -c eval.cpp -o eval.o

josssubs.o : josssubs.cpp joss.hpp Makefile
	$(CC) $(CFLAGS) -c josssubs.cpp -o josssubs.o

clean:
	rm -f joss *.o *~
