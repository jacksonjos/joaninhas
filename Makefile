CC=gcc
CFLAGS=-Wall -std=c11 -pedantic -O4

main: joaninhas.c
	@$(CC) $(CFLAGS) joaninhas.c -o joaninhas -fopenmp

gprof: joaninhas.c
	@$(CC) $(CFLAGS) joaninhas.c -o joaninhas -Wno-unknown-pragmas -pg

clean:
	@rm -rf joaninhas
