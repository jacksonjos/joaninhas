#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define NADA -1
#define CALOR -2
#define FRIO -3

struct hex {
	int id;
	unsigned int semente;
	double temperatura;
	int n; /* Duração de ciclos das fontes de calor e frio. */
};

struct movimentacao {
	int i_atual, j_atual, i_novo, j_novo, movimenta;
	double delta;
};

struct fonte {
	int x, y, tipo;
};

extern int rand_r (unsigned int *__seed) __THROW; /* Senão o compilador reclama que não está declarada. */
void init();
void imprime();
void sorteia_fonte_calor_ou_frio(struct hex *hex);
double distancia(int lin1, int col1, int lin2, int col2);
void calcula_temperatura(int i, int j);
void inspeciona_vizinho_quando_esta_frio(int i, int j);
void inspeciona_vizinho_quando_esta_quente(int i, int j);

int L, A, J, nc, nf, T, P;
unsigned int s;
double C, Tmin, Tmax, pc, pf;
struct hex **hexes;
struct movimentacao *movimentacao;
struct fonte *fontes; /* Guarda as posições das joaninhas, fontes de calor e frio */
int iv, jv; /* Guarda a posição do melhor hexágono vizinho para a joaninha se mover. */
int topo; /* Guarda o tamanho atual do vetor das fontes de temperatura */

int main(int argc, char **argv) {
	int iter, i, j, quem_movimenta, empate;

	if (argc != 14) {
		fprintf(stderr, "Usage: %s L A J s C Tmin Tmax pc nc pf nf T P\n", argv[0]);
		exit(1);
	}
	else {
		L    = atoi(argv[1]);  /* Largura da matriz. */
		A    = atoi(argv[2]);  /* Altura da matriz. */
		J    = atoi(argv[3]);  /* Número de joaninhas. */
		s    = atoi(argv[4]);  /* Semente para o gerador aleatório. */
		C    = atof(argv[5]);  /* Constante de emissão de calor da joaninha. */
		Tmin = atof(argv[6]);  /* Menor temperatura que a joaninha considera confortável. */
		Tmax = atof(argv[7]);  /* Maior temperatura que a joaninha considera confortável. */
		pc   = atof(argv[8]);  /* Probabilidade, por hexágono, de aparecer uma fonte de calor. */
		nc   = atoi(argv[9]);  /* Duração em ciclos da fonte de calor. */
		pf   = atof(argv[10]); /* Probabilidade, por hexágono, de aparecer uma fonte de frio. */
		nf   = atoi(argv[11]); /* Duração em ciclos da fonte de frio. */
		T    = atoi(argv[12]); /* Número de ciclos de simulação. */
		P    = atoi(argv[13]); /* Número de processadores (threads) para execução. */
	}

	omp_set_num_threads(P);
	init();
	/*imprime();*/

	/* A simulação acontece aqui. */
	for (iter = 0; iter < T; iter++) {
		topo = 0;
		/* Sorteia fontes de calor e frio. */
		for (i = 0; i < L; i++)
			for (j = 0; j < A; j++) {
				switch(hexes[i][j].id) {
					case NADA:
						sorteia_fonte_calor_ou_frio(&hexes[i][j]);
						break;
					
					case CALOR:
					case FRIO:
						hexes[i][j].n--;
						if (hexes[i][j].n == -1)
							hexes[i][j].id = NADA;

					default:
						fontes[topo].x = i;
						fontes[topo].y = j;
						fontes[topo++].tipo = hexes[i][j].id;
						break;
				}
			}

		/* Joaninhas. */
		for (i = 0; i < L; i++) {
			for (j = 0; j < A; j++) {
				if (hexes[i][j].id >= 0) {
					calcula_temperatura(i, j);
					/* Joaninha quer se mover, calcula temperatura dos vizinhos. */
					iv = i; jv = j; /* Por enquanto a joaninha permanece onde está. */
					if (hexes[i][j].temperatura < Tmin) {
						if (i % 2 == 0) { /* Linha par. */
							if ((i-1 >= 0) && (j+1 < L))
								inspeciona_vizinho_quando_esta_frio(i-1, j+1);
							if ((i+1 < A) && (j+i < L))
								inspeciona_vizinho_quando_esta_frio(i+1, j+1);
						}
						else { /* Linha ímpar. */
							if ((i-1 >= 0) && (j-1 >= 0))
								inspeciona_vizinho_quando_esta_frio(i-1, j-1);
							if ((i+1 < A) && (j+i < L))
								inspeciona_vizinho_quando_esta_frio(i+1, j-1);
						}
						/* Linha tanto par quanto ímpar. */

						if (i+1 < A)
							inspeciona_vizinho_quando_esta_frio(i+1, j);
						if (i-1 >= 0)
							inspeciona_vizinho_quando_esta_frio(i-1, j);
						if (j+1 < L)
							inspeciona_vizinho_quando_esta_frio(i, j+1);
						if (j-1 >= 0)
							inspeciona_vizinho_quando_esta_frio(i, j-1);

						/* Atualiza movimentacao. */
						movimentacao[hexes[i][j].id].movimenta = 1;
						movimentacao[hexes[i][j].id].i_atual = i;
						movimentacao[hexes[i][j].id].j_atual = j;
						movimentacao[hexes[i][j].id].i_novo = iv;
						movimentacao[hexes[i][j].id].j_novo = jv;
						movimentacao[hexes[i][j].id].delta = hexes[i][j].temperatura - hexes[iv][jv].temperatura;
						if (movimentacao[hexes[i][j].id].delta < 0) movimentacao[hexes[i][j].id].delta *= -1;
					}
					else if (hexes[i][j].temperatura > Tmax) {
						if (i % 2 == 0) { /* Linha par. */
							if ((i-1 >= 0) && (j+1 < L))
								inspeciona_vizinho_quando_esta_quente(i-1, j+1);
							if ((i+1 < A) && (j+i < L))
								inspeciona_vizinho_quando_esta_quente(i+1, j+1);
						}
						else { /* Linha ímpar. */
							if ((i-1 >= 0) && (j-1 >= 0))
								inspeciona_vizinho_quando_esta_quente(i-1, j-1);
							if ((i+1 < A) && (j+i < L))
								inspeciona_vizinho_quando_esta_quente(i+1, j-1);
						}
						/* Linha tanto par quanto ímpar. */

						if (i+1 < A)
							inspeciona_vizinho_quando_esta_quente(i+1, j);
						if (i-1 >= 0)
							inspeciona_vizinho_quando_esta_quente(i-1, j);
						if (j+1 < L)
							inspeciona_vizinho_quando_esta_quente(i, j+1);
						if (j-1 >= 0)
							inspeciona_vizinho_quando_esta_quente(i, j-1);

						/* Atualiza movimentacao. */
						movimentacao[hexes[i][j].id].movimenta = 1;
						movimentacao[hexes[i][j].id].i_atual = i;
						movimentacao[hexes[i][j].id].j_atual = j;
						movimentacao[hexes[i][j].id].i_novo = iv;
						movimentacao[hexes[i][j].id].j_novo = jv;
						movimentacao[hexes[i][j].id].delta = hexes[i][j].temperatura - hexes[iv][jv].temperatura;
						if (movimentacao[hexes[i][j].id].delta < 0) movimentacao[hexes[i][j].id].delta *= -1;
					}
					else {
						movimentacao[hexes[i][j].id].movimenta = 0; /* Joaninha não se movimenta. */
					}
				}
			}
		}
		/*imprime()*/
		/* As movimentações são feitas aqui. */
		for (i = 0; i < J; i++) {
			if (movimentacao[i].movimenta) {
				empate = 0;
				quem_movimenta = i;
				for (j = i; j < J; j++) {
					if (movimentacao[i].i_novo == movimentacao[j].i_novo && movimentacao[i].j_novo == movimentacao[j].j_novo) { /* Conflito. */
						if (movimentacao[i].delta == movimentacao[j].delta) {
							empate = 1;
							movimentacao[j].movimenta = 0;
						}
						else if (movimentacao[i].delta < movimentacao[j].delta) {
							empate = 0;
							movimentacao[i].movimenta = 0;
							quem_movimenta = j;
						}
						else movimentacao[j].movimenta = 0;
					}
				}
				if (empate) {
					movimentacao[quem_movimenta].movimenta = 0; /* Se há empate ninguém se movimenta. */
				}
				else {
					/* Faz a movimentação. */
					hexes[movimentacao[quem_movimenta].i_novo][movimentacao[quem_movimenta].j_novo].id = hexes[movimentacao[quem_movimenta].i_atual][movimentacao[quem_movimenta].j_atual].id;
					hexes[movimentacao[quem_movimenta].i_novo][movimentacao[quem_movimenta].j_novo].temperatura = hexes[movimentacao[quem_movimenta].i_atual][movimentacao[quem_movimenta].j_atual].temperatura;
					hexes[movimentacao[quem_movimenta].i_atual][movimentacao[quem_movimenta].j_atual].id = NADA;
					movimentacao[quem_movimenta].movimenta = 0;
				}
			}
		}
	}
	/* Atualiza fontes de calor e frio. */
	for (i = 0; i < L; i++)
		for (j = 0; j < A; j++)
			if (hexes[i][j].id == CALOR || hexes[i][j].id == FRIO) {
				hexes[i][j].n--;
				if (hexes[i][j].n == -1) hexes[i][j].id = NADA;
			}
	imprime();

	return 0;
}

void init() {
	int i, j, ii, jj;

	hexes = malloc(L*sizeof(struct hex *));
	movimentacao = malloc(J*sizeof(struct movimentacao));
	fontes = malloc((L*A) * sizeof(struct fonte));

	for (i = 0; i < L; i++) {
		hexes[i] = malloc(A*sizeof(struct hex));
		for (j = 0; j < A; j++) {
			hexes[i][j].id = NADA;
			hexes[i][j].n = 0;
			hexes[i][j].temperatura = 0.0;			
			hexes[i][j].semente = ((i + 1)*s + j) % RAND_MAX;
		}
	}

	srand(s);
	for (i = 0; i < J; i++) {
		ii = rand() % L;
		jj = rand() % A;
		if (hexes[ii][jj].id >= 0) i--; /* Executa a mesma iteração outra vez. */
		else hexes[ii][jj].id = i;
	}
}

void imprime() {
	int i, j;

	for (i = 0; i < L; i++) {
		for (j = 0; j < A; j++) {
			if (hexes[i][j].id == NADA) printf("|            ");
			else if (hexes[i][j].id == CALOR) printf("|      +     ");
			else if (hexes[i][j].id == FRIO) printf("|      -     ");
			else printf("| %7.2lf %2d ", hexes[i][j].temperatura, hexes[i][j].id);
		}
		printf("|\n");
	}
	printf("\n");
}

void sorteia_fonte_calor_ou_frio(struct hex *hex) {
	/* Fonte de calor deve ser sorteada primeiro. */
	if ((double) rand_r(&hex->semente)/RAND_MAX <= pc) {
		hex->id = CALOR;
		hex->n = nc;
	}
	else if ((double) rand_r(&hex->semente)/RAND_MAX <= pf) {
		hex->id = FRIO;
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

void calcula_temperatura(int i, int j) {
	int ii, jj;
	double temperatura, d;

	temperatura = 0;
	for (ii = 0; ii < L; ii++) {
		for (jj = 0; jj < A; jj++) {
			if (i == ii && j == jj) continue;
			if (hexes[ii][jj].id > 0 || hexes[ii][jj].id == CALOR) {
				d = distancia(i, j, ii, jj);
				temperatura += C/(d*d);
			}
			else if (hexes[ii][jj].id == FRIO) {
				d = distancia(i, j, ii, jj);
				temperatura -= C/(d*d);
			}
		}
	}
	hexes[i][j].temperatura = temperatura;
}

void inspeciona_vizinho_quando_esta_frio(int i, int j) {
	if (hexes[i][j].id == NADA) {
		calcula_temperatura(i, j);
		if (hexes[i][j].temperatura > hexes[iv][jv].temperatura) {
			iv = i;
			jv = j;
		}
	}
}

void inspeciona_vizinho_quando_esta_quente(int i, int j) {
	if ( hexes[i][j].id == NADA) {
		calcula_temperatura(i, j);
		if (hexes[i][j].temperatura < hexes[iv][jv].temperatura) {
			iv = i;
			jv = j;
		}
	}
}
