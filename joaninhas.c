#include <stdio.h>
#include <stdlib.h>
#include <math.h>
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
void init();
void imprime();
void sorteia_fonte_calor_ou_frio(struct hex *hex);
double distancia(int lin1, int col1, int lin2, int col2);
void calcula_temperatura(struct hex *hex);

int L, A, J, nc, nf, T, P;
unsigned int s;
double C, Tmin, Tmax, pc, pf;
struct hex **hexes;
double melhor_vizinho_i, melhor_vizinho_j;

int main(int argc, char **argv) {
	int iter, ii, jj;

	if (argc != 14) {
		fprintf(stderr, "Usage: %s L A J s C Tmin Tmax pc nc pf nf T P\n", argv[0]);
		exit(1);
	}
	else {
		/*
		L    largura da matriz
		A    altura da matriz
		J    número de joaninhas
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
		J = atoi(argv[3]);
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

	init();
	imprime();

	/* a simulação acontece aqui */
	for (iter = 0; iter < T; iter++) {
		/* sorteia fontes de calor e frio */
		for (ii = 0; ii < L; ii++)
			for (jj = 0; jj < A; jj++)
				if (hexes[ii][jj].tipo == NADA)
					sorteia_fonte_calor_ou_frio(&hexes[ii][jj]);

		/* joaninhas */
		for (ii = 0; ii < L; ii++) {
			for (jj = 0; jj < A; jj++) {
				if (hexes[ii][jj].tipo == JOANINHA) {
					calcula_temperatura(&hexes[ii][jj]);
					/* joaninha quer se mover, calcula temperatura dos vizinhos */
					/* aqui temos que calcular uma pré-movimentação da joaninha */
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

		imprime();
	}

	return 0;
}

void init() {
	int i, j, ii, jj;
    
    hexes = malloc(L*sizeof(struct hex *));
    for (i = 0; i < L; i++) {
	    hexes[i] = malloc(A*sizeof(struct hex));
        for (j = 0; j < A; j++) {
            hexes[i][j].semente = ((i + 1)*s + j) % RAND_MAX;
        }
    }
    srand(s);

	for (i = 0; i < J; i++) {
		ii = rand() % L;
		jj = rand() % A;
		if (hexes[ii][jj].tipo == JOANINHA) i--; /* executa mesma iteração outra vez */
		else hexes[ii][jj].tipo = JOANINHA;
	}
}

void imprime() {
	int i, j;

	for (i = 0; i < L; i++) {
		for (j = 0; j < A; j++) {
			printf("| %11u ", hexes[i][j].semente);
		}
		printf("|\n");
	}
	printf("\n");
}

void sorteia_fonte_calor_ou_frio(struct hex *hex) {
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

double distancia(int lin1, int col1, int lin2, int col2) {
	int par1, par2;

	par1 = lin1 % 2;
	par2 = lin2 % 2;

	if (par1 == par2) return sqrt(pow(abs(col1-col2), 2) + 0.75*pow(abs(lin1-lin2), 2));
	if (par1 == 0) return sqrt(pow(abs(col1-col2+0.5), 2) + 0.75*pow(abs(lin1-lin2), 2));
	/* par1 == 1 */ return sqrt(pow(abs(col1-col2-0.5), 2) + 0.75*pow(abs(lin1-lin2), 2));
}

void calcula_temperatura(struct hex *hex) {
}
