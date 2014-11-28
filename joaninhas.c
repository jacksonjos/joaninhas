#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

enum tipos {
	NADA,
	JOANINHA,
	CALOR,
	FRIO
};

typedef enum tipos hex_tipos;

struct hex {
	hex_tipos tipo;
	unsigned int semente;
	double temperatura;
	int n; /* duração de ciclos das fontes de calor e frio */
};

extern int rand_r (unsigned int *__seed) __THROW; /* senão o compilador idiota reclama que não está declarada ¬¬' */
struct hex **init(int s, int L, int A, int num_joaninhas);
void imprime(struct hex **hexes, int L, int A);
void sorteia_fonte_calor_ou_frio(struct hex *hex, double pc, int nc, double pf, int nf);
void calcula_temperatura(struct hex *hex, double C);

int main(int argc, char **argv) {
	int L, A, j, nc, nf, T, P;
	unsigned int s;
	double C, Tmin, Tmax, pc, pf;
	struct hex **hexes;
	int iter, ii, jj;

	if (argc != 14) {
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
		C = atof(argv[5]);
		Tmin = atof(argv[6]);
		Tmax = atof(argv[7]);
		pc = atof(argv[8]);
		nc = atoi(argv[9]);
		pf = atof(argv[10]);
		nf = atoi(argv[11]);
		T = atoi(argv[12]);
		P = atoi(argv[13]);
	}
	omp_set_num_threads(P);

	hexes = init(s, L, A, j);
	imprime(hexes, L, A);

	/* a simulação acontece aqui */
	for (iter = 0; iter < T; iter++) {
		/* sorteia fontes de calor e frio */
		for (ii = 0; ii < L; ii++)
			for (jj = 0; jj < A; jj++)
				if (hexes[ii][jj].tipo == NADA)
					sorteia_fonte_calor_ou_frio(&hexes[ii][jj], pc, nc, pf, nf);

		/* joaninhas */
		for (ii = 0; ii < L; ii++) {
			for (jj = 0; jj < A; jj++) {
				if (hexes[ii][jj].tipo == JOANINHA) {
					calcula_temperatura(&hexes[ii][jj], C);
					if (hexes[ii][jj].temperatura < Tmin || hexes[ii][jj].temperatura > Tmax) {
						/* joaninha quer se mover, calcula temperatura dos vizinhos */
						if (ii % 2 == 0) { /* linha par */
							calcula_temperatura(&hexes[ii-1][jj], C);
							calcula_temperatura(&hexes[ii-1][jj+1], C);
							calcula_temperatura(&hexes[ii][jj-1], C);
							calcula_temperatura(&hexes[ii][jj+1], C);
							calcula_temperatura(&hexes[ii+1][jj], C);
							calcula_temperatura(&hexes[ii+1][jj+1], C);
						}
						else { /*linha impar */
							calcula_temperatura(&hexes[ii-1][jj-1], C);
							calcula_temperatura(&hexes[ii-1][jj], C);
							calcula_temperatura(&hexes[ii][jj-1], C);
							calcula_temperatura(&hexes[ii][jj+1], C);
							calcula_temperatura(&hexes[ii+1][jj-1], C);
							calcula_temperatura(&hexes[ii+1][jj], C);
						}
						/* aqui temos que calcular uma pré-movimentação da joaninha */
					}
				}
			}
		}
		/* aqui precisamos resolver os conflitos de movimentação */

		/* atualiza fontes de calor e frio */
		for (ii = 0; ii < L; ii++)
			for (jj = 0; jj < A; jj++)
				if (hexes[ii][jj].tipo == CALOR || hexes[ii][jj].tipo == FRIO) {
					hexes[ii][jj].n--;
					if (hexes[ii][jj].n == 0) hexes[ii][jj].tipo = NADA;
				}

		imprime(hexes, L, A);
	}

	return 0;
}

struct hex **init(int s, int L, int A, int num_joaninhas) {
	int i, j, ii, jj;
	struct hex **hexes;

	hexes = malloc(L*sizeof(struct hex *));
	for (i = 0; i < L; i++) {
		hexes[i] = malloc(A*sizeof(struct hex));
		for (j = 0; j < A; j++) {
			hexes[i][j].semente = ((i + 1)*s + j) % RAND_MAX;
			hexes[i][j].temperatura = 4;
		}
	}
	srand(s);
	for (i = 0; i < num_joaninhas; i++) {
		ii = rand() % L;
		jj = rand() % A;
		if (hexes[ii][jj].tipo == JOANINHA) i--; /* executa mesma iteração outra vez */
		else hexes[ii][jj].tipo = JOANINHA;
	}

	return hexes;
}

void imprime(struct hex **hexes, int L, int A) {
	int i, j;

	for (i = 0; i < L; i++) {
		for (j = 0; j < A; j++) {
			printf("| %11u ", hexes[i][j].semente);
		}
		printf("|\n");
	}
	printf("\n");
}

void sorteia_fonte_calor_ou_frio(struct hex *hex, double pc, int nc, double pf, int nf) {
	/* fonte de calor deve ser sorteada primeiro */
	if ((double) rand_r(&hex->semente)/RAND_MAX <= pc) {
		hex->tipo = CALOR;
		hex->n = nc;
	}
	else if ((double) rand_r(&hex->semente)/RAND_MAX <= pf) {
		hex->tipo = FRIO;
		hex->n = nf;
	}
}

void calcula_temperatura(struct hex *hex, double C) {
}
