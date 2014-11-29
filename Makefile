CC=gcc
CFLAGS=-Wall -std=c11 -pedantic -O3 -fopenmp

main: joaninhas.c
	$(CC) $(CFLAGS) joaninhas.c -o joaninhas -lm

gprof: joaninhas.c
	$(CC) $(CFLAGS) joaninhas.c -o joaninhas -pg

clean:
	@rm -rf joaninhas
