/*
 ============================================================================
 Name		 : Programação, e-Fólio de Recurso
 Author		 : David Salsinha
 Description : Gerador de Quadrados Mágicos, alíneas A-D
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <limits.h>

/*
 * Constantes:
 * - valor minimo para o lado de um quadrado
 * - mensagem a apresentar em caso de erro de alocação de memória
 */
#define INSTRUCOES_UTILIZACAO "Utilizacao: %s <tamanho> [alinea]"
#define MINIMO_LADO 3
#define ERRO_INPUT "tem de ser igual ou superior a 3"
#define ERRO_MEMORIA "ERRO: Nao foi possivel alocar memoria a necessaria"

/*
 * Função "wrapper" para malloc() que exibe mensagem de erro e sai do
 * programa em caso de falha de alocação de memória
 */
void* _malloc(size_t tamanho) {
	void* apontador = malloc(tamanho);
	if(apontador == NULL) {
		printf(ERRO_MEMORIA);
		exit(EXIT_FAILURE);
	}
	return apontador;
}

/*
 * Função que troca os valores de dois locais de memória especificados
 * pelos apontadores a e b
 */
void _trocar_valores(int* a, int* b) {
	int temp;
	temp = *a;
	*a = *b;
	*b = temp;
}

/*
 * Função que percorre um array e, por cada elemento, troca os valores
 * entre o atual e outro de indice aleatório
 */
void _baralhar_array(int* array, int n_elementos) {
	int i, aleatorio;
	for(i = 0; i < n_elementos; i++) {
		aleatorio = rand() % n_elementos;
		_trocar_valores(array + i, array + aleatorio);
	}
}

/*
 * Função que devolve o valor da soma mágica, baseado num valor especificado
 * para a dimensão lateral do quadrado
 */
int _soma_magica(int lado) {
	return (int) floor(lado * (lado * lado + 1) / 2);
}

/*
 * Estrutura que representa um gerador de quadrados numéricos
 */
typedef struct _gerador_quadrados_numericos {
	int lado; /* dimensão lateral dos quadrados */
	int area; /* numero total de elementos */
	int* numeros; /* array com os numeros que compõem o quadrado */

	/* apontador para uma função que determina se o estado atual é uma solução*/
	bool (* funcao_validacao)(struct _gerador_quadrados_numericos*);

	int solucoes; /* numero de soluções encontradas */
	int soma_magica; /* valor da soma que determina se o quadrado é mágico */
} GeradorQuadradosNumericos;

/*
 * typedefs que definem a "assinatura" das funções de validação:
 * - função de validação de soluções; devolve true ou false caso o quadrado seja
 *   ou não uma solução
 * - função chamada em cada ciclo que gera uma permutação dos numeros; recebe a
 *   posição atual e retorna true ou false caso a permutação parcial deva ou não
 *   continuar a ser explorada
 */
typedef bool (GeradorQuadradosNumericos_FuncaoValidacao)
												(GeradorQuadradosNumericos*);
typedef bool (GeradorQuadradosNumericos_FuncaoPermutacao)
											(GeradorQuadradosNumericos*, int);

/*
 * Função construtora da estrutura GeradorQuadradosNumericos, recebe o valor da
 * dimensão lateral, a indicação se deve iniciar com os numeros ordenados de
 * forma sequencial ou se deverão ser "baralhados". Recebe ainda um apontador
 * para a função de validação de soluções.
 */
GeradorQuadradosNumericos* GeradorQuadradosNumericos_new(int lado,bool baralhar,
				GeradorQuadradosNumericos_FuncaoValidacao* funcao_validacao) {
	GeradorQuadradosNumericos* this;
	int area = lado * lado; /* pré-determina o numero total de elementos */
	int i;

	if(lado < MINIMO_LADO)
		return NULL;

	this = _malloc(sizeof(GeradorQuadradosNumericos));
	this->lado = lado;
	this->area = area;
	this->numeros = _malloc(area * sizeof(int));

	/* inicializa o array com a sequencia de numeros */
	for(i = 0; i < this->area; i++)
		this->numeros[i] = i + 1;

	if(baralhar)
		_baralhar_array(this->numeros, this->area);

	this->funcao_validacao = funcao_validacao;

	this->solucoes = 0;
	this->soma_magica = _soma_magica(lado);

	return this;
}

/*
 * Função que liberta a memória ocupada pela estrutura
 */
void GeradorQuadradosNumericos_free(GeradorQuadradosNumericos* this) {
	free(this->numeros);
	free(this);
}

/*
 * Função que faz output de um quadrado numérico
 */
void GeradorQuadradosNumericos_print(GeradorQuadradosNumericos* this) {
	int i, j;
	for(i = 0; i < this->lado; i++) {
		for(j = 0; j < this->lado; j++)
			printf("%4d ", this->numeros[i * this->lado + j]);
		printf("\n");
	}
}

/*
 * Função auxiliar recursiva que gera permutação de todos os elementos do array
 * Recebe o indice do elemento atual até onde está fixa a permutação parcial e
 * um apontador para uma função que determina se devem continuar a ser geradas
 * as restantes permutações a partir daquele ponto
 */
void _GeradorQuadradosNumericos_permutar(GeradorQuadradosNumericos* this, int i,
				GeradorQuadradosNumericos_FuncaoPermutacao* funcao_permutacao) {
	int j;

	/*
	 * Como a chamada recursiva incrementa sempre 1 ao i, eventualmente chegará
	 * a um valor igual ao numero de elementos, ou seja, gerou uma permutação
	 * completa
	 */
	if(i == this->area && this->funcao_validacao(this))
		return;
	/*
	 * Caso contrário, ainda é uma permutação parcial que se explora caso não
	 * esteja uma função de validação parcial definida ou caso a mesma retorne
	 * true
	 */
	else if(i < this->area &&
				(funcao_permutacao == NULL || funcao_permutacao(this, i)) ) {
		for (j = i; j < this->area; j++) {
			/* troca os valores entre a posição atual e a próxima */
			_trocar_valores(this->numeros + i, this->numeros + j);
			/* explora recursivamente os próximos passos da permutação */
			_GeradorQuadradosNumericos_permutar(this,i+1,funcao_permutacao);
			/* volta atrás para seguir para a próxima permutação parcial */
			_trocar_valores(this->numeros + i, this->numeros + j);
		}
	}
}

/*
 * Função que percorre todas as permutações de um quadrado numérico, com a
 * possibilidade de passar um apontador para uma função que valida a permutação
 * de forma a poder diminuir o espaço de procura de soluções
 */
void GeradorQuadradosNumericos_permutar(GeradorQuadradosNumericos* this,
				GeradorQuadradosNumericos_FuncaoPermutacao* funcao_permutacao) {
	/* chama a função recursiva, com inicio no elemento de indice zero */
	_GeradorQuadradosNumericos_permutar(this, 0, funcao_permutacao);
}

/* Alinea A: Mostra em output o valor da soma mágica correspondente a um
 * quadrado mágico de lado N
 */
void Alinea_A(int n) {
	printf("Soma magica: %d", _soma_magica(n));
}

/*
 * Alinea B: Mostra em output um quadrado aleatório, que pode ou não ser mágico
 * mas sem o validar. Em seguida é escrito o mesmo que na alinea anterior, ou
 * seja, o valor da soma mágica correspondente ao quadrado exibido
 */
void Alinea_B(int n) {
	/* Novo quadrado aleatorio com dimensão n, aleatório e sem validação */
	GeradorQuadradosNumericos* aleatorio = GeradorQuadradosNumericos_new(n,true,
																		NULL);
	GeradorQuadradosNumericos_print(aleatorio);
	GeradorQuadradosNumericos_free(aleatorio);
	Alinea_A(n);
}

/*
 * Função de validação para um quadrado numérico, verificando se corresponde
 * a um quadrado mágico
 */
bool _validar_quadrado_magico(GeradorQuadradosNumericos* quadrado) {
	int i, j, soma1, soma2;

	/* linhas e colunas */
	for(i = 0; i < quadrado->lado; i++) {
		soma1 = soma2 = 0;
		for(j = 0; j < quadrado->lado; j++) {
			soma1 += quadrado->numeros[i * quadrado->lado + j];
			soma2 += quadrado->numeros[j * quadrado->lado + i];
		}
		if(soma1 != soma2 || soma1 != quadrado->soma_magica)
			return false;
	}

	/* diagonais */
	soma1 = soma2 = 0;
	for(i = 0, j = quadrado->lado - 1; i < quadrado->area && j < quadrado->area;
							i += quadrado->lado + 1, j += quadrado->lado - 1) {
		soma1 += quadrado->numeros[i];
		soma2 += quadrado->numeros[j];
	}
	if(soma1 != soma2 || soma1 != quadrado->soma_magica)
		return false;

	/* chegando aqui, pode-se considerar que é solução */
	quadrado->solucoes++;
	printf("Quadrado magico #%d:\n", quadrado->solucoes);
	GeradorQuadradosNumericos_print(quadrado);

	return true;
}

/*
 * Alinea C: Percorre todas as permutações e mostra as que são soluções
 */
void Alinea_C(int n) {
	/* não aleatório e com função de validação */
	GeradorQuadradosNumericos* permutacoes = GeradorQuadradosNumericos_new(n,
											false, _validar_quadrado_magico);
	/* percorre todas as permutações sem função que valide as parciais */
	GeradorQuadradosNumericos_permutar(permutacoes, NULL);
	GeradorQuadradosNumericos_free(permutacoes);
}

/*
 * Função de validação de permutações parciais, com k elementos colocados
 */
bool _validar_permutacao(GeradorQuadradosNumericos* permutacao, int k) {
	int i, j, linha, coluna, soma;

	/* calculo da linha e coluna a que corresponde o k */
	linha = (int) (k / permutacao->lado);
	coluna = k % permutacao->lado;

	/* caso tenha percorrido uma linha completa, verifica as somas das linhas */
	if(k > 0 && coluna == 0) {
		for(i = 0; i < linha; i++) {
			soma = 0;
			for(j = 0; j < permutacao->lado; j++) {
				soma += permutacao->numeros[i * permutacao->lado + j];
			}
			if(soma != permutacao->soma_magica)
				return false;
		}
	}

	return true;
}

/*
 * Alinea C: Percorre todas as permutações e mostra as que são soluções, com
 * recurso a verificações parciais para melhorar a procura
 */
void Alinea_D(int n) {
	/* aleatório e com função de validação */
	GeradorQuadradosNumericos* permutacoes = GeradorQuadradosNumericos_new(n,
												true, _validar_quadrado_magico);
	/* percorre todas as permutações com função de validação de parciais */
	GeradorQuadradosNumericos_permutar(permutacoes, _validar_permutacao);
	GeradorQuadradosNumericos_free(permutacoes);
}

/*
 * main
 */
int main(int argc, char** argv) {
	int n = 0;
	srand(time(NULL)); /* inicialização do gerador de numeros aleatórios */

	if(argc == 1) /* sem argumentos */
		printf(INSTRUCOES_UTILIZACAO, argv[0]);
	else if(argc > 1) { /* 1 ou mais argumentos */
		n = (int) strtol(argv[1], NULL, 10);
		if(n < MINIMO_LADO) {
			printf(ERRO_INPUT);
			return EXIT_FAILURE;
		} else {
			if(argc > 2) { /* o 2º argumento especifica a alinea */
				switch(tolower(*argv[2])) {
					case 'a':
						Alinea_A(n);
						break;
					case 'b':
						Alinea_B(n);
						break;
					case 'c':
						Alinea_C(n);
						break;
					case 'd':
						Alinea_D(n);
						break;
				}
			} else /* sem o 2º argumento, é executada a alinea D */
				Alinea_D(n);
		}
	}
	return EXIT_SUCCESS;
}
