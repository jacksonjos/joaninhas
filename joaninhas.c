#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
	int L, A, j, s, C, nc, nf, T, P;
	double Tmin, Tmax, pc, pf;
	if (argc != 14) {
		fprintf(stderr, "Usage: %s L A j s C Tmin Tmax pc nc pf nf T P\n", argv[0]);
		exit(1);
	}
	else {
		L = atoi(argv[1]);
		A = atoi(argv[2]);
		j = atoi(argv[3]);
		s = atoi(argv[4]);
		C = atoi(argv[5]);
		Tmin = atof(argv[6]);
		Tmax = atof(argv[7]);
		pc = atof(argv[8]);
		nc = atoi(argv[9]);
		pf = atof(argv[10]);
		nf = atoi(argv[11]);
		T = atoi(argv[12]);
		P = atoi(argv[13]);
	}

	return 0;
}
