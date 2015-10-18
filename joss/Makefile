# Makefile for the JOSS implementation.

CC = g++ 

joss : joss.o parse.o eval.o josssubs.o Makefile
	$(CC) $(CFLAGS) -o joss joss.o parse.o eval.o josssubs.o -lm

joss.o : joss.c joss.h Makefile
	$(CC) $(CFLAGS) -c joss.c -o joss.o

parse.o : parse.c joss.h Makefile
	$(CC) $(CFLAGS) -c parse.c -o parse.o

eval.o : eval.c joss.h Makefile
	$(CC) $(CFLAGS) -c eval.c -o eval.o

josssubs.o : josssubs.c joss.h Makefile
	$(CC) $(CFLAGS) -c josssubs.c -o josssubs.o

clean:
	rm -f joss *.o *~
