#include <stdio.h>
#include <stdlib.h>

enum tipos {
    NADA,
    JOANINHA,
    FRIO,
    CALOR
};

typedef enum tipos hex_tipos;

struct hex {
    double temperatura;
    hex_tipos tipo;
    int semente;
};

void init(struct hex **hexes, int s, int L, int A);
void imprime(struct hex **hexes, int L, int A);

int main(int argc, char **argv) {
	int L, A, j, s, C, nc, nf, T, P;
	double Tmin, Tmax, pc, pf;
	struct hex **hexes;
	
	if (argc != 0) {
		fprintf(stderr, "Usage: %s L A j s C Tmin Tmax pc nc pf nf T P\n", argv[0]);
		exit(1);
	}
	else {
		/*
		L    largura da matriz
		A    altura da matriz
		j    número de joaninhas
		s    semente para o gerador aleatório
		C    constante de emissão de calor da joaninha
		Tmin menor temperatura que a joaninha considera confortável
		Tmax maior temperatura que a joaninha considera confortável
		pc   probabilidade, por hexágono, de aparecer uma fonte de calor
		nc   duração em ciclos da fonte de calor
		pf   probabilidade, por hexágono, de aparecer uma fonte de frio
		nf   duração em ciclos da fonte de frio
		T    número de ciclos de simulação
		P    número de processadores (threads) para execução
		*/
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

	init(hexes, s, L, A);
	imprime(hexes, L, A);

	return 0;
}

void init(struct hex **hexes, int s, int L, int A) {
    int i, j;
    
    hexes = (struct hex**) malloc(L * sizeof(struct hex*));
	
    srand(s);
    
	printf("2:%d\n", s);
    
    for (i = 0; i < L; i++) {
	    hexes[i] = malloc(A * sizeof (struct hex));
        for (j = 0; j < A; j++) {
            hexes[i][j].semente = ((i+1) * rand()+j) % RAND_MAX;
            hexes[i][j].temperatura = 4;
        }
        printf("%d:%f\n", i, hexes[i][j].temperatura);
    }
}

void imprime(struct hex **hexes, int L, int A) {
    int i, j;
    
    for (i = 0; i < L; i++) {
        for (j = 0; j < A; j++) {
            printf("| %d ", hexes[i][j].semente);
        }
        printf("|\n");
    }
    printf("\n");
}

