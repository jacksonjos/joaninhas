CC=gcc
CFLAGS=-Wall -std=c11 -pedantic -O4 -fopenmp

main: joaninhas.c
	$(CC) $(CFLAGS) joaninhas.c -o joaninhas -lm

gprof: joaninhas.c
	$(CC) $(CFLAGS) joaninhas.c -o joaninhas -lm -pg

clean:
	@rm -rf joaninhas
