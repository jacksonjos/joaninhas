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
void calcula_temperatura_vizinho_min_e_atualiza_iv_jv(int i, int j);
void calcula_temperatura_vizinho_max_e_atualiza_iv_jv(int i, int j);

int L, A, J, nc, nf, T, P;
unsigned int s;
double C, Tmin, Tmax, pc, pf;
struct hex **hexes;
int iv, jv; /* guarda a posição da melhor casa vizinha para a joaninha se mover */

int main(int argc, char **argv) {
	int iter, i, j;

	if (argc != 14) {
		fprintf(stderr, "Usage: %s L A J s C Tmin Tmax pc nc pf nf T P\n", argv[0]);
		exit(1);
	}
	else {
		L    = atoi(argv[1]);  /* largura da matriz */
		A    = atoi(argv[2]);  /* altura da matriz */
		J    = atoi(argv[3]);  /* número de joaninhas */
		s    = atoi(argv[4]);  /* semente para o gerador aleatório */
		C    = atof(argv[5]);  /* constante de emissão de calor da joaninha */
		Tmin = atof(argv[6]);  /* menor temperatura que a joaninha considera confortável */
		Tmax = atof(argv[7]);  /* maior temperatura que a joaninha considera confortável */
		pc   = atof(argv[8]);  /* probabilidade, por hexágono, de aparecer uma fonte de calor */
		nc   = atoi(argv[9]);  /* duração em ciclos da fonte de calor */
		pf   = atof(argv[10]); /* probabilidade, por hexágono, de aparecer uma fonte de frio */
		nf   = atoi(argv[11]); /* duração em ciclos da fonte de frio */
		T    = atoi(argv[12]); /* número de ciclos de simulação */
		P    = atoi(argv[13]); /* número de processadores (threads) para execução */
	}

	omp_set_num_threads(P);
	init();
	imprime();

	/* a simulação acontece aqui */
	for (iter = 0; iter < T; iter++) {
		/* sorteia fontes de calor e frio */
		for (i = 0; i < L; i++)
			for (j = 0; j < A; j++)
				if (hexes[i][j].tipo == NADA)
					sorteia_fonte_calor_ou_frio(&hexes[i][j]);

		/* joaninhas */
		for (i = 0; i < L; i++) {
			for (j = 0; j < A; j++) {
				if (hexes[i][j].tipo == JOANINHA) {
					calcula_temperatura(&hexes[i][j]);
					/* joaninha quer se mover, calcula temperatura dos vizinhos */
					iv = i; jv = j; /* por enquanto a joaninha permanece onde está */
					if (hexes[i][j].temperatura < Tmin) {
						if (i % 2 == 0) { /* linha par */
							calcula_temperatura_vizinho_min_e_atualiza_iv_jv(i-1, j);
							calcula_temperatura_vizinho_min_e_atualiza_iv_jv(i-1, j+1);
							calcula_temperatura_vizinho_min_e_atualiza_iv_jv(i+1, j+1);
						}
						else { /* linhas impar */
							calcula_temperatura_vizinho_min_e_atualiza_iv_jv(i-1, j-1);
							calcula_temperatura_vizinho_min_e_atualiza_iv_jv(i-1, j);
							calcula_temperatura_vizinho_min_e_atualiza_iv_jv(i+1, j-1);
						}
						/* ambas linhas */
						calcula_temperatura_vizinho_min_e_atualiza_iv_jv(i+1, j);
						calcula_temperatura_vizinho_min_e_atualiza_iv_jv(i, j+1);
						calcula_temperatura_vizinho_min_e_atualiza_iv_jv(i, j-1);
					}
					else if (hexes[i][j].temperatura > Tmax) {
						if (i % 2 == 0) { /* linha par */
							calcula_temperatura_vizinho_max_e_atualiza_iv_jv(i-1, j);
							calcula_temperatura_vizinho_max_e_atualiza_iv_jv(i-1, j+1);
							calcula_temperatura_vizinho_max_e_atualiza_iv_jv(i+1, j+1);
						}
						else { /* linhas impar */
							calcula_temperatura_vizinho_max_e_atualiza_iv_jv(i-1, j-1);
							calcula_temperatura_vizinho_max_e_atualiza_iv_jv(i-1, j);
							calcula_temperatura_vizinho_max_e_atualiza_iv_jv(i+1, j-1);
						}
						/* ambas linhas */
						calcula_temperatura_vizinho_max_e_atualiza_iv_jv(i+1, j);
						calcula_temperatura_vizinho_max_e_atualiza_iv_jv(i, j+1);
						calcula_temperatura_vizinho_max_e_atualiza_iv_jv(i, j-1);
					}
					/* temos o melhor candidato em iv, jv; agora temos que calcular uma pré-movimentação da joaninha */
				}
			}
		}
		/* aqui precisamos resolver os conflitos de movimentação */

		/* atualiza fontes de calor e frio */
		for (i = 0; i < L; i++)
			for (j = 0; j < A; j++)
				if (hexes[i][j].tipo == CALOR || hexes[i][j].tipo == FRIO) {
					hexes[i][j].n--;
					if (hexes[i][j].n == 0) hexes[i][j].tipo = NADA;
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

void calcula_temperatura_vizinho_min_e_atualiza_iv_jv(int i, int j) {
	if (i >= 0 && i <= L && j >= 0 && j <= A && hexes[i][j].tipo == NADA) {
		calcula_temperatura(&hexes[i][j]);
		if (hexes[i][j].temperatura > hexes[iv][jv].temperatura) {
			iv = i;
			jv = j;
		}
	}
}

void calcula_temperatura_vizinho_max_e_atualiza_iv_jv(int i, int j) {
	if (i >= 0 && i <= L && j >= 0 && j <= A && hexes[i][j].tipo == NADA) {
		calcula_temperatura(&hexes[i][j]);
		if (hexes[i][j].temperatura < hexes[iv][jv].temperatura) {
			iv = i;
			jv = j;
		}
	}
}
