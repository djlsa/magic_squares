/*
 ============================================================================
 Name		 : Programa��o, e-F�lio de Recurso
 Author		 : David Salsinha
 Description : Gerador de Quadrados M�gicos, al�neas A-D
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
 * - mensagem a apresentar em caso de erro de aloca��o de mem�ria
 */
#define INSTRUCOES_UTILIZACAO "Utilizacao: %s <tamanho> [alinea]"
#define MINIMO_LADO 3
#define ERRO_INPUT "tem de ser igual ou superior a 3"
#define ERRO_MEMORIA "ERRO: Nao foi possivel alocar memoria a necessaria"

/*
 * Fun��o "wrapper" para malloc() que exibe mensagem de erro e sai do
 * programa em caso de falha de aloca��o de mem�ria
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
 * Fun��o que troca os valores de dois locais de mem�ria especificados
 * pelos apontadores a e b
 */
void _trocar_valores(int* a, int* b) {
	int temp;
	temp = *a;
	*a = *b;
	*b = temp;
}

/*
 * Fun��o que percorre um array e, por cada elemento, troca os valores
 * entre o atual e outro de indice aleat�rio
 */
void _baralhar_array(int* array, int n_elementos) {
	int i, aleatorio;
	for(i = 0; i < n_elementos; i++) {
		aleatorio = rand() % n_elementos;
		_trocar_valores(array + i, array + aleatorio);
	}
}

/*
 * Fun��o que devolve o valor da soma m�gica, baseado num valor especificado
 * para a dimens�o lateral do quadrado
 */
int _soma_magica(int lado) {
	return (int) floor(lado * (lado * lado + 1) / 2);
}

/*
 * Estrutura que representa um gerador de quadrados num�ricos
 */
typedef struct _gerador_quadrados_numericos {
	int lado; /* dimens�o lateral dos quadrados */
	int area; /* numero total de elementos */
	int* numeros; /* array com os numeros que comp�em o quadrado */

	/* apontador para uma fun��o que determina se o estado atual � uma solu��o*/
	bool (* funcao_validacao)(struct _gerador_quadrados_numericos*);

	int solucoes; /* numero de solu��es encontradas */
	int soma_magica; /* valor da soma que determina se o quadrado � m�gico */
} GeradorQuadradosNumericos;

/*
 * typedefs que definem a "assinatura" das fun��es de valida��o:
 * - fun��o de valida��o de solu��es; devolve true ou false caso o quadrado seja
 *   ou n�o uma solu��o
 * - fun��o chamada em cada ciclo que gera uma permuta��o dos numeros; recebe a
 *   posi��o atual e retorna true ou false caso a permuta��o parcial deva ou n�o
 *   continuar a ser explorada
 */
typedef bool (GeradorQuadradosNumericos_FuncaoValidacao)
												(GeradorQuadradosNumericos*);
typedef bool (GeradorQuadradosNumericos_FuncaoPermutacao)
											(GeradorQuadradosNumericos*, int);

/*
 * Fun��o construtora da estrutura GeradorQuadradosNumericos, recebe o valor da
 * dimens�o lateral, a indica��o se deve iniciar com os numeros ordenados de
 * forma sequencial ou se dever�o ser "baralhados". Recebe ainda um apontador
 * para a fun��o de valida��o de solu��es.
 */
GeradorQuadradosNumericos* GeradorQuadradosNumericos_new(int lado,bool baralhar,
				GeradorQuadradosNumericos_FuncaoValidacao* funcao_validacao) {
	GeradorQuadradosNumericos* this;
	int area = lado * lado; /* pr�-determina o numero total de elementos */
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
 * Fun��o que liberta a mem�ria ocupada pela estrutura
 */
void GeradorQuadradosNumericos_free(GeradorQuadradosNumericos* this) {
	free(this->numeros);
	free(this);
}

/*
 * Fun��o que faz output de um quadrado num�rico
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
 * Fun��o auxiliar recursiva que gera permuta��o de todos os elementos do array
 * Recebe o indice do elemento atual at� onde est� fixa a permuta��o parcial e
 * um apontador para uma fun��o que determina se devem continuar a ser geradas
 * as restantes permuta��es a partir daquele ponto
 */
void _GeradorQuadradosNumericos_permutar(GeradorQuadradosNumericos* this, int i,
				GeradorQuadradosNumericos_FuncaoPermutacao* funcao_permutacao) {
	int j;

	/*
	 * Como a chamada recursiva incrementa sempre 1 ao i, eventualmente chegar�
	 * a um valor igual ao numero de elementos, ou seja, gerou uma permuta��o
	 * completa
	 */
	if(i == this->area && this->funcao_validacao(this))
		return;
	/*
	 * Caso contr�rio, ainda � uma permuta��o parcial que se explora caso n�o
	 * esteja uma fun��o de valida��o parcial definida ou caso a mesma retorne
	 * true
	 */
	else if(i < this->area &&
				(funcao_permutacao == NULL || funcao_permutacao(this, i)) ) {
		for (j = i; j < this->area; j++) {
			/* troca os valores entre a posi��o atual e a pr�xima */
			_trocar_valores(this->numeros + i, this->numeros + j);
			/* explora recursivamente os pr�ximos passos da permuta��o */
			_GeradorQuadradosNumericos_permutar(this,i+1,funcao_permutacao);
			/* volta atr�s para seguir para a pr�xima permuta��o parcial */
			_trocar_valores(this->numeros + i, this->numeros + j);
		}
	}
}

/*
 * Fun��o que percorre todas as permuta��es de um quadrado num�rico, com a
 * possibilidade de passar um apontador para uma fun��o que valida a permuta��o
 * de forma a poder diminuir o espa�o de procura de solu��es
 */
void GeradorQuadradosNumericos_permutar(GeradorQuadradosNumericos* this,
				GeradorQuadradosNumericos_FuncaoPermutacao* funcao_permutacao) {
	/* chama a fun��o recursiva, com inicio no elemento de indice zero */
	_GeradorQuadradosNumericos_permutar(this, 0, funcao_permutacao);
}

/* Alinea A: Mostra em output o valor da soma m�gica correspondente a um
 * quadrado m�gico de lado N
 */
void Alinea_A(int n) {
	printf("Soma magica: %d", _soma_magica(n));
}

/*
 * Alinea B: Mostra em output um quadrado aleat�rio, que pode ou n�o ser m�gico
 * mas sem o validar. Em seguida � escrito o mesmo que na alinea anterior, ou
 * seja, o valor da soma m�gica correspondente ao quadrado exibido
 */
void Alinea_B(int n) {
	/* Novo quadrado aleatorio com dimens�o n, aleat�rio e sem valida��o */
	GeradorQuadradosNumericos* aleatorio = GeradorQuadradosNumericos_new(n,true,
																		NULL);
	GeradorQuadradosNumericos_print(aleatorio);
	GeradorQuadradosNumericos_free(aleatorio);
	Alinea_A(n);
}

/*
 * Fun��o de valida��o para um quadrado num�rico, verificando se corresponde
 * a um quadrado m�gico
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

	/* chegando aqui, pode-se considerar que � solu��o */
	quadrado->solucoes++;
	printf("Quadrado magico #%d:\n", quadrado->solucoes);
	GeradorQuadradosNumericos_print(quadrado);

	return true;
}

/*
 * Alinea C: Percorre todas as permuta��es e mostra as que s�o solu��es
 */
void Alinea_C(int n) {
	/* n�o aleat�rio e com fun��o de valida��o */
	GeradorQuadradosNumericos* permutacoes = GeradorQuadradosNumericos_new(n,
											false, _validar_quadrado_magico);
	/* percorre todas as permuta��es sem fun��o que valide as parciais */
	GeradorQuadradosNumericos_permutar(permutacoes, NULL);
	GeradorQuadradosNumericos_free(permutacoes);
}

/*
 * Fun��o de valida��o de permuta��es parciais, com k elementos colocados
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
 * Alinea C: Percorre todas as permuta��es e mostra as que s�o solu��es, com
 * recurso a verifica��es parciais para melhorar a procura
 */
void Alinea_D(int n) {
	/* aleat�rio e com fun��o de valida��o */
	GeradorQuadradosNumericos* permutacoes = GeradorQuadradosNumericos_new(n,
												true, _validar_quadrado_magico);
	/* percorre todas as permuta��es com fun��o de valida��o de parciais */
	GeradorQuadradosNumericos_permutar(permutacoes, _validar_permutacao);
	GeradorQuadradosNumericos_free(permutacoes);
}

/*
 * main
 */
int main(int argc, char** argv) {
	int n = 0;
	srand(time(NULL)); /* inicializa��o do gerador de numeros aleat�rios */

	if(argc == 1) /* sem argumentos */
		printf(INSTRUCOES_UTILIZACAO, argv[0]);
	else if(argc > 1) { /* 1 ou mais argumentos */
		n = (int) strtol(argv[1], NULL, 10);
		if(n < MINIMO_LADO) {
			printf(ERRO_INPUT);
			return EXIT_FAILURE;
		} else {
			if(argc > 2) { /* o 2� argumento especifica a alinea */
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
			} else /* sem o 2� argumento, � executada a alinea D */
				Alinea_D(n);
		}
	}
	return EXIT_SUCCESS;
}
