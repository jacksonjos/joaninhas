#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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

struct hex **init(int s, int L, int A, int num_joaninhas);
void imprime(struct hex **hexes, int L, int A);
void sorteia_fonte_calor_ou_frio(struct hex *hex, double pc, int nc, double pf, int nf);
double distancia(int lin1, int col1, int lin2, int col2);

int main(int argc, char **argv) {
	int L, A, j, C, nc, nf, T, P;
	unsigned int s;
	double Tmin, Tmax, pc, pf;
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
/*
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
		printf("%d %d %d %u %d %lf %lf %lf %d %lf %d %d %d\n\n", L, A, j, s, C, Tmin, Tmax, pc, nc, pf, nf, T, P);
	}
	*/
	
	s = 80;
	L = 4;
	A = 6;
	j = 9;

	hexes = init(s, L, A, j);
	imprime(hexes, L, A);

	/* a simulação acontece aqui */
	for (iter = 0; iter < T; iter++) {
		/* sorteia fontes de calor e frio */
		for (ii = 0; ii < L; ii++) {
			for (jj = 0; jj < A; jj++) {
				/* quando não há nada no hexágono, ou quando havia uma fonte que apagou na última rodada */
				if (hexes[ii][jj].tipo == NADA ||
						((hexes[ii][jj].tipo == CALOR || hexes[ii][jj].tipo == FRIO) && hexes[ii][jj].n == 0)) {
						sorteia_fonte_calor_ou_frio(&hexes[ii][jj], pc, nc, pf, nf);
				}
			}
		}


		/* calcula temperatura das joaninhas */
		for (ii = 0; ii < L; ii++) {
			for (jj = 0; jj < A; jj++) {
			}
		}
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
}

void sorteia_fonte_calor_ou_frio(struct hex *hex, double pc, int nc, double pf, int nf) {
	srand(hex->semente);
	if ((double) rand()/RAND_MAX <= pc) {
		hex->tipo = CALOR;
		hex->n = nc;
	}
	else if ((double) rand()/RAND_MAX <= pf) {
		hex->tipo = FRIO;
		hex->n = nf;
	}
}

double distancia(int lin1, int col1, int lin2, int col2) {
	int par1, par2;
	
	par1 = lin1 % 2;
	par2 = lin2 % 2;

	if (par1 == par2) {
		return sqrt(pow(abs(col1-col2), 2) + ((1/sqrt(3))*1.5*pow(abs(lin1-lin2), 2)));
		
	} else {
		
		if (par1 == 0) {
			if (col1 > col2) {
				return sqrt(pow(abs(col1-col2), 2) + ((1/sqrt(3))*1.5*pow(abs(lin1-lin2), 2)));
			} else {
				return sqrt(pow(abs(col1-col2+0.5), 2) + ((1/sqrt(3))*1.5*pow(abs(lin1-lin2), 2)));
			}
		} else {
			if (par1 == 1) {
				if (col1 > col2) {
					return sqrt(pow(abs(col1-col2-0.5), 2) + ((1/sqrt(3))*1.5*pow(abs(lin1-lin2), 2)));
				} else {
					return sqrt(pow(abs(col1-col2-0.5), 2) + ((1/sqrt(3))*1.5*pow(abs(lin1-lin2), 2)));
				}
			}
		}
	}
	return 0;
}		
