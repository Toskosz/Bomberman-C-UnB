/*
Universidade de Brasilia
Instituto de Ciencias Exatas
Departamento de Ciencia da Computacao
Algoritmos   e   Programação   de   Computadores   –
2/2019
Aluno(a): Thiago Masera Tokarski
Matricula: 190096063
Turma: A
Versão do compilador: gcc version 7.4.0 ubuntu

Descricao: Jogo bomberman feito em ligaguem C para terminal de linux e windows
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#ifdef _WIN32
#define CLEAR "cls"
#else 
#define CLEAR "clear"
#endif

#define MEDIA_TEMPO 7
#define ERRO_TEMPO 3
#define RAND(M, E) (M + (rand()%(2*E + 1)- E))

/* variavel que verifica se o jogador está vivo*/
int morreu = 0;

/*estrutura de condiçoes da partida*/
typedef struct estadoPartida
{
	char nomeJogador[20];
	char tabuleiro[15][28];
	int tempo;
	int tempo_partida;
	int linha_j;
	int coluna_j;
	int bomba;
} estadoPartida;

/*estrutura do ranking da partida atual*/
typedef struct
{
	int tempo_ranking;
	char nome_ranking[20];
}tipoRanking;

/*inicializacao das funcoes usadas*/
void verifica_pos_desejada(estadoPartida* m, int x, int y);
void randomiza_mapa(estadoPartida* m);
int imprime_tela_inicial();
void printa_mapa(estadoPartida* m);
void lemapa(estadoPartida* m);
int tempo_e_instrucoes(int tempo);
void planta_bomba(estadoPartida* m, int x, int y);
int cronometro_bomba(estadoPartida* m);
void boom(estadoPartida* m, int l, int c);
void limpa_posicao(estadoPartida* m, int l, int c);
void localiza_e_explode(estadoPartida* m);
void verifica_tempo_bomba(estadoPartida* m);
void acha_jogador(estadoPartida* m);
void identifica_pos_desejada(estadoPartida* m, char movimento);
void movimenta_jogador(estadoPartida* m, char movimento);
void jogo(estadoPartida* m, int tempo_restante);
void carrega_jogo(estadoPartida* m);
void salva_jogo(estadoPartida* m);
void menu(estadoPartida* m);
int tem_monstros(estadoPartida* m);
void salva_ranking(estadoPartida* m, char arquivo[]);
void carrega_ranking(estadoPartida* m, char arquivo[]);
int qntd_pessoas_ranking(char arquivo[]);
void recebe_ranking(tipoRanking* vet, char arquivo[], int qntd);
void ordena_ranking(tipoRanking* vet, int qntd);
void printa_ranking(tipoRanking* vet, int qntd);

/*função para coletar a quantidade de pessoas presentes no arquivo ranking.txt*/
int qntd_pessoas_ranking(char arquivo[])
{
	char ch;
	int contador = 1;
	FILE* f;
	f = fopen(arquivo, "r");
	if (f == NULL)
	{
		return -1;
	}
	while((ch=fgetc(f))!=EOF) /*while para verificar final de arquivo*/
	{
      if(ch=='\n')
      {
      	contador++;
      }
   	}
   	return contador;
}

/*funcao para salvar jogo no ranking*/
void salva_ranking(estadoPartida* m, char arquivo[])
{
	FILE* f;
	f = fopen(arquivo, "r+");
	if (f == NULL)
	{
		/*criar arquivo caso este nao exista*/
		f = fopen(arquivo, "w");
	}
	fseek(f, 0, SEEK_END); /*pula direto para o final do arquivo*/
	fprintf(f, "%s %d\n", m->nomeJogador, m->tempo_partida); /*printa jogador no ranking*/
	fclose(f);
}

/*funcao para alocacao dinamica do ranking*/
void carrega_ranking(estadoPartida* m, char arquivo[])
{
	int t = qntd_pessoas_ranking(arquivo);
	/* t == -1 logo não tem ranking*/
	if(t == -1)
	{
		printf("Nao temos um ranking ainda :C\n");
		printf("Pressione <enter> para voltar ao menu.\n");
		getchar();
		getchar();
		system(CLEAR);
		menu(m);
	}
	tipoRanking* vet;
	vet = (tipoRanking*) malloc(sizeof(tipoRanking)*t);
	recebe_ranking(vet, arquivo, t);
	free(vet);
	menu(m);
}
/*recebe os rankings do arquivo txt e aloca em um vetor de estruturas de tipoRanking*/
void recebe_ranking(tipoRanking* vet, char arquivo[], int qntd)
{
	int i;
	FILE* f;
	f = fopen(arquivo, "r");
	for (i = 0; i < qntd; ++i)
	{
		fscanf(f, "%s %d", vet[i].nome_ranking, &vet[i].tempo_ranking);
	}
	ordena_ranking(vet, qntd);
	printa_ranking(vet, qntd);
}

/*funcao para printar o vetor já ordenado do ranking*/
void printa_ranking(tipoRanking* vet, int qntd)
{
	int i;
	printf("****************************** RANKING *************************************\n\n");
	for (i = 0; i < qntd; i++)
	{
		printf("%d\t%s\t\t%d\n", i+1, vet[i].nome_ranking, vet[i].tempo_ranking);
	}
	printf("\nTecle <enter> para retornar ao menu");
	getchar();
	getchar();
	system(CLEAR);
}
/*funcao para ordernar de acordo com a pontuacao*/
void ordena_ranking(tipoRanking* vet, int qntd)
{
	int i, j;
	for (i = 0; i < qntd; i++)
	{
		for (j = 0; j < qntd; j++)
		{
			if (vet[j].tempo_ranking < vet[i].tempo_ranking)
			{
				/*bubblesort*/
				int tmp = vet[i].tempo_ranking;
				vet[i].tempo_ranking = vet[j].tempo_ranking;            
				vet[j].tempo_ranking = tmp;
				char nome_tmp[20];
				strcpy(nome_tmp, vet[i].nome_ranking);
				strcpy(vet[i].nome_ranking, vet[j].nome_ranking);
				strcpy(vet[j].nome_ranking, nome_tmp);                    
			}
		}
	}
}

/*funcao para salvar o jogo no arquivo bin*/
void salva_jogo(estadoPartida* m)
{
	FILE* f;
	f = fopen("jogo.bin", "wb");
	fwrite(m, sizeof(struct estadoPartida), 1, f);
	printf("Jogo salvo!\n");
	fclose(f);
	printf("Aperte <enter> para sair.\n");
	getchar();
	getchar();
}
/*funcao para carregar jogo do arquivo bin*/
void carrega_jogo(estadoPartida* m)
{
	FILE* f;
	f = fopen("jogo.bin", "rb");
	if (f == NULL)
	{
		printf("Nenhum jogo salvo encontrado.\nPressione <enter> para voltar ao menu");
		getchar();
		getchar();
		system(CLEAR);
		menu(m);
	}
	/*armazena oque foi lido na estrutra de partida atual*/
	fread(m, sizeof(struct estadoPartida), 1, f);
	printf("Jogo carregado!\nAperte <enter> para inicar de onde parou.");
	getchar();
	getchar();
	fclose(f);
}

/*funcao para verificar se ainda tem monstro no mapa*/
int tem_monstros(estadoPartida* m)
{
	int i, j;
	for (i = 0; i < 15; ++i)
	{
		for (j = 0; j < 28; ++j)
		{
			if (m->tabuleiro[i][j] == '@')
			{
				return 1;
			}
		}
	}
	return 0;
}

/* funcao do proprio jogo*/
void jogo(estadoPartida* m, int tempo_restante)
{
	/* variavel para guardar o movimento do jogador*/
	char movimento;
	do
	{
		m->tempo_partida = tempo_e_instrucoes(tempo_restante);
		printa_mapa(m);

		scanf(" %c", &movimento);

		/* --------tratamento da entrada do usuario-----------*/
		if (movimento > 90)
		{
			movimento -= 32;
		}
		/*---------------aviso de bomba ativa-----------------*/
		if (movimento == 'B' && m->bomba == 1)
		{
			system(CLEAR);
			printf("Uma bomba ainda esta ativa!\n");
			continue;
		}
		/* opcao de sair do jogo*/
		if (movimento == 'E')
		{
			salva_jogo(m);
			break;
		}
		movimenta_jogador(m, movimento);
		/*verifica tempo da bomba*/
		if(m->bomba == 1)
		{
			verifica_tempo_bomba(m);
		}
		system(CLEAR);
	} while (m->tempo_partida != 0 && morreu == 0 && tem_monstros(m) != 0);
	/*------------------Mensagens para os possiveis finais -----------------*/
	/* dois getchar para limpar o buffer*/
	if(m->tempo_partida <= 0)
	{
		printa_mapa(m);
		printf("TEMPO ESGOTADO\n.\n.\n.\n");
		printf("VOCE PERDEU :C\n");
		printf("tecle <enter> para encerrar\n");
		getchar();
		getchar();
	} else if (morreu == 1)
	{
		printa_mapa(m);
		printf("VOCE MORREU\n.\n.\n.\n");
		printf("QUEM SABE NA PROXIMA :D\n");
		printf("tecle <enter> para encerrar\n");
		getchar();
		getchar();
	} else if (tem_monstros(m) == 0)
	{
		printa_mapa(m);
		printf("PARABENS!\n.\n.\n.\n");
		printf("Voce venceu o jogo!\n\n");
		printf("Pontuação salva no ranking!\n");
		printf("tecle <enter> para encerrar\n");
		salva_ranking(m, "ranking.txt");
		getchar();
		getchar();
	}
	/*-----------------------------------------------------------------------*/
}

/*Inicializa o mapa na variavel tabuleiro*/
void lemapa(estadoPartida* m)
{
	int i;
	for (i = 0; i < 15; ++i)
	{
		if (i == 0)
		{
			strcpy(m->tabuleiro[i], " _________________________ ");
		}else if(i == 14)
		{
			strcpy(m->tabuleiro[i], "|_________________________|");
		}else if (i % 2 != 0)
		{
			strcpy(m->tabuleiro[i], "|                         |");
		} else 
		{
			strcpy(m->tabuleiro[i], "| + + + + + + + + + + + + |");
		}
	}
}

void printa_mapa(estadoPartida* m)
{
	int i;
	for (i = 0; i < 15; ++i)
	{
		printf("%s\n", m->tabuleiro[i]);
	}
}

int imprime_tela_inicial()
{
	int opcao;
	printf("******************** BOMBERMAN ********************\n\n\n");
	printf("1. JOGAR\n");
	printf("2. CONTINUAR JOGO\n");
	printf("3. RANKING\n");
	printf("4. SAIR\n\n");
	printf("Opcao: ");
	scanf("%d", &opcao);
	return opcao;
}

/*funcao pra randomizacao do mapa*/
void randomiza_mapa(estadoPartida* m)
{
	int i, j;
	/*posiona o jogador*/
	m->tabuleiro[13][1] = '&';
	for (i = 1; i < 14; ++i)
	{
		/*limite de 1 mosntro por linha para o mapa nao ficar poluido*/
		int limite_de_monstros = 1;
		/*array com os possiveis elementos de randomizacao*/
		char elementos[3] = {" @#"};
		for (j = 1; j < 26; ++j) 
		{
			if (m->tabuleiro[i][j] == '+' || m->tabuleiro[i][j] == '&' )
			{
				/* eh parede ou jogador entao nao faz nada 
				sequencias de if para impedir de o jogar ficar preso entre monstros e paredes*/
			}else if (i == 12 && j == 1)
			{}else if (i == 11 && j == 1)
			{}else if (i == 13 && j == 2)
			{}else if (i == 13 && j == 3)
			{} else 
			{
				/* imprimi aleatoriamente algum elemento */
				int n_aleatorio = rand()%3;
				m->tabuleiro[i][j] = elementos[n_aleatorio];

				/* limita a quantidade de monstros */
				if (m->tabuleiro[i][j] == '@')
				{
					limite_de_monstros--;
					if (limite_de_monstros == 0)
					{
						elementos[1] =' ';
					}
				}
			}
		}
	}
}

int tempo_e_instrucoes(int tempo)
{
	int tempo_restante = tempo - time(0);
	printf("TEMPO RESTANTE: %d\n\n", tempo_restante);
	printf("A - ESQUERDA    D - DIREITA    S - CIMA\n");
	printf("X - BAIXO       B - BOMBA      E - ENCERRAR\n");
	return tempo_restante;	
}

/* acha a linha e coluna atual do jogador e atribui ah linha_j e coluna_j*/
void acha_jogador(estadoPartida* m)
{
	int i, j;
	for (i = 0; i < 15; ++i)
	{
		for (j = 0; j < 28; ++j)
		{
			if (m->tabuleiro[i][j] == '&')
			{
				m->linha_j = i;
				m->coluna_j = j;
			}
		}
	}
}

/*verifica a pos_desejada pelo jogador*/
void verifica_pos_desejada(estadoPartida* m, int x, int y)
{
	if (m->tabuleiro[x][y] == '|' || m->tabuleiro[x][y] == '_'
		|| m->tabuleiro[x][y] == '+' || m->tabuleiro[x][y] == '#')
	{
		/* paredes entao nao faz nada */
	} else if (m->tabuleiro[x][y] == '@')
	{
		/*monstro entao morre*/
		morreu = 1;
		m->tabuleiro[m->linha_j][m->coluna_j] = ' ';
	} else if (m->tabuleiro[x][y] == ' ')
	{
		/*posicao livre*/
		m->tabuleiro[x][y] = '&';
		/*if para caso o jogador tenha botado uma bomba no movimento anterior*/
		if (m->tabuleiro[m->linha_j][m->coluna_j] == '*')
		{
		} else 
		{
			m->tabuleiro[m->linha_j][m->coluna_j] = ' ';
		}
	}
}

/* recebe o movimento e calcula e verifica a posicao desejada*/
void identifica_pos_desejada(estadoPartida* m, char movimento)
{
	int linha_desejada;
	int coluna_desejada;
	if (movimento == 'A')
	{
		linha_desejada = m->linha_j;
	 	coluna_desejada = m->coluna_j - 1;
	 	verifica_pos_desejada(m, linha_desejada, coluna_desejada);
	} else if (movimento == 'D')
	{
		linha_desejada = m->linha_j;
		coluna_desejada = m->coluna_j + 1;
		verifica_pos_desejada(m, linha_desejada, coluna_desejada);
	} else if (movimento == 'S')
	{
		linha_desejada = m->linha_j - 1;
		coluna_desejada = m->coluna_j;
		verifica_pos_desejada(m, linha_desejada, coluna_desejada);
	} else if (movimento == 'X')
	{
		linha_desejada = m->linha_j + 1;
		coluna_desejada = m->coluna_j;
		verifica_pos_desejada(m, linha_desejada, coluna_desejada);
	} else if (movimento == 'B')
	{
		linha_desejada = m->linha_j;
		coluna_desejada = m->coluna_j;
		planta_bomba(m, linha_desejada, coluna_desejada);
	} else 
	{
		linha_desejada = m->linha_j;
		coluna_desejada = m->coluna_j;
		verifica_pos_desejada(m, linha_desejada, coluna_desejada);
	}
}

void planta_bomba(estadoPartida* m, int x, int y)
{
	/*coloca a bomba no mapa]*/
	m->tabuleiro[x][y] = '*';
	m->bomba = 1;
	/*inicia a contagem de tempo da bomba*/
	m->tempo = time(0) + RAND(MEDIA_TEMPO, ERRO_TEMPO);
}

void movimenta_jogador(estadoPartida* m, char movimento)
{
	/*acha o jogador e atribui sua linha e coluna nas variaveis linha_j e coluna_j*/
	acha_jogador(m);
	identifica_pos_desejada(m, movimento);
}

/* funcao para explodir a bomba*/
void localiza_e_explode(estadoPartida* m)
{
	int i, j;
	for (i = 0; i < 15; ++i)
	{
		for (j = 0; j < 28; ++j)
		{
			if (m->tabuleiro[i][j] == '*')
			{
				boom(m, i, j);
			}
		}
	}
}

/*verifica e apaga os possiveis espacos ao redor da bomba*/
void boom(estadoPartida* m, int l, int c)
{
	limpa_posicao(m, l-1, c);
	limpa_posicao(m, l-1, c+1);
	limpa_posicao(m, l-1, c-1);
	limpa_posicao(m, l+1, c); 
	limpa_posicao(m, l, c+1);
	limpa_posicao(m, l, c-1);
	limpa_posicao(m, l+1, c-1);
	limpa_posicao(m, l+1, c+1);
	m->bomba = 0;
	m->tabuleiro[l][c] = ' ';
}

/*funcao que limpa a matriz nos espacos onde a bomba explodiu*/
void limpa_posicao(estadoPartida* m, int l, int c)
{
	if (m->tabuleiro[l][c] == '_' || m->tabuleiro[l][c] == '+')
	{
	} else if (m->tabuleiro[l][c] == '#')
	{
		m->tabuleiro[l][c] = ' ';
	} else if (m->tabuleiro[l][c] == '@')
	{
		m->tabuleiro[l][c] = ' ';
	} else if (m->tabuleiro[l][c] == '&')
	{
		morreu = 1;
		m->tabuleiro[l][c] = ' ';
	}
}

/*calcula tempo restante para  bomba explodir */
int cronometro_bomba(estadoPartida* m)
{
	int bomba_restante;
	bomba_restante = m->tempo - time(0);
	return bomba_restante;
}
/*verifica o tempo restante de bomba*/
void verifica_tempo_bomba(estadoPartida* m)
{
	int tempo_bomba;
	tempo_bomba = cronometro_bomba(m);
	if (tempo_bomba <= 0)
	{
		localiza_e_explode(m);
	}
}

/*funcao do menu*/
void menu(estadoPartida* m)
{
	int opcao;	
	opcao = imprime_tela_inicial();
	if (opcao == 1)
	{
		system(CLEAR);
		printf("Digite seu nome e ele ficará salvo em nosso ranking.\n");
		scanf("%s", m->nomeJogador);
		system(CLEAR);
		lemapa(m);
		randomiza_mapa(m);
		/* tempo restante de partida*/
		int tempo_restante;
		tempo_restante = time(0) + 200;
		jogo(m, tempo_restante);
	} 
	else if (opcao == 2)
	{
		system(CLEAR);
		carrega_jogo(m);
		/* tempo restante de partida*/
		int tempo_restante;
		tempo_restante = time(0) + m->tempo_partida;
		jogo(m, tempo_restante);
	} else if (opcao == 3)
	{
		system(CLEAR);
		carrega_ranking(m, "ranking.txt");
	} else if (opcao == 4)
	{
		system(CLEAR);
		printf("Volte sempre :D\n");
	}
}

int main ()
{
	srand(time(0));
	/*incializacao da estrutura principal*/
	estadoPartida atual;
	menu(&atual);
	return 0;
}