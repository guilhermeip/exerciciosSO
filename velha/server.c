/*
Autor: Guilherme Ianhez Pereira dos Santos
data: 03/11/2018
*/
#include <netdb.h>      /* dados sobre a rede (ex.: hosts, protocolos, ...) */
#include <netinet/in.h> /* fun��es, tipos e constantes para sockets (Internet) */
#include <stdio.h>      /* entrada/sa�da (I/O) */
#include <stdlib.h>     /* exit() */
#include <string.h>     /* manipula��o de strings */
#include <sys/socket.h> /* fun��es, tipos e constantes para sockets (UNIX) */
#include <sys/types.h>  /* tipos b�sicos do GNU C */
#include <sys/wait.h>   /* tipos b�sicos do GNU C */
#include <unistd.h>     /*Fork*/
#include <unistd.h>     /* close() */
#include "velha.c"

#define IP_SERVER "127.0.0.1" /* especifica o IP do servidor */
#define PORTA_PADRAO 8085     /* especifica a porta padrao de conexao */
#define MAX_CLIENT 2          /* maximo de conexoes */
#define MAX_BUFFER 100        /* tamanho m�ximo do buffer */

void enviarMsg(char msg[], int fd) {
	int rc = strlen(msg);
	if (write(fd, msg, rc) != rc) {
		if (rc > 0)
			fprintf(stderr, "partial write");
		else {
			perror("write error");
			exit(-1);
		}
	}
}
int main(int argc, char *argv[]) {
	struct hostent *ptrh;           /* ponteiro para a tabela de hosts */
	struct sockaddr_in addr_server; /* estrutura para armazenar o IP e a porta do servidor */
	struct sockaddr_in addr_client; /* estrutura para armazenar o endereco do cliente */
	int serverSocket;               /* socket para ouvir conex�es */
	int playerSocket;               /* socket para comunica��o com o jogador conectado */
	int port;                       /* porta utilizada na comunica��o */

	/* verifica os par�metros passados por linha de comando */
	if (argc > 1)
		port = atoi(argv[1]); /* converte para inteiro */
	else
		port = PORTA_PADRAO; /* usa a porta padr�o */

	/* testa a validade da porta */
	if ((port <= 1024) || (port >= 65536)) {
		fprintf(stderr, "n�mero de porta inv�lido %s\n", argv[2]);
		exit(1);
	}  //if

	/* obtem o endereco do servidor (isto �, o endere�o local) */
	ptrh = gethostbyname(IP_SERVER);

	/* preenche com zeros a estrutura sockaddr_in */
	memset((char *)&addr_server, 0, sizeof(addr_server));

	/* preenche a estrutura endereco */
	addr_server.sin_family = AF_INET;         /* define familia como Internet  */
	addr_server.sin_port = htons(port);       /* define a porta */
	addr_server.sin_addr.s_addr = INADDR_ANY; /* seta o endereco IP local */
	//memcpy(&addr_server.sin_addr, ptrh->h_addr, ptrh->h_length);   /* especifica o IP */

	/* cria um socket servidor TCP */
	serverSocket = socket(PF_INET, SOCK_STREAM, 0);

	/* testa para verificar sucesso */
	if (serverSocket < 0) {
		fprintf(stderr, "falha na cria��o do socket\n");
		exit(1);
	}  //if

	/* mapeia um endereco local para o socket */
	if (bind(serverSocket, (struct sockaddr *)&addr_server, sizeof(addr_server)) < 0) {
		fprintf(stderr, "erro no mapeamento da porta (bind)\n");
		exit(1);
	}  //if

	/* Especifica o numero de conexoes suportadas */
	if (listen(serverSocket, MAX_CLIENT) < 0) {
		fprintf(stderr, "erro em limitar o n�mero de conex�es (listen)\n");
		exit(1);
	}  //if
	//controle de jogadores conectados
	int jogadorConectado = 0;
	//pipe para comunicação dos filhos
	int pipeServidor[2][2];
	int i = 0;
	int pid;

	//cria o primeiro pipe para conversa.
	if (pipe(pipeServidor[0])) {
		fprintf(stderr, "Falha ao criar o Pipe.\n");
		return EXIT_FAILURE;
	}

	//segundo pipe
	if (pipe(pipeServidor[1])) {
		fprintf(stderr, "Falha ao criar o Pipe.\n");
		return EXIT_FAILURE;
	}
	//inicia a velha para os 2 filhos
	inicio_velha();
	for (i = 0; i < 2; ++i) {
		//aguarda conexoes
		int szAddressClient = sizeof(addr_client);
		printf("Servidor iniciado e aguardando conex�es ...\n");
		if ((playerSocket = accept(serverSocket, (struct sockaddr *)&addr_client, (socklen_t *)&szAddressClient)) < 0) {
			fprintf(stderr, "erro ao aceitar conex�o (accept)\n");
			exit(1);
		}  // if
		printf("Cliente conectado!\nPlayerSocket: #%d#\n", playerSocket);
		++jogadorConectado;
		pid = fork();

		if (pid > 0) { /* Sou pai, criarei mais filhos */
			continue;
		} else if (pid == 0) { /* Sou filho, vou trabalhar */
			if (jogadorConectado == 1) {
				//jogador 1 vai escutar pelo pipe [0][0] e escrever pelo [1][1]
				close(pipeServidor[0][1]);  //Ouve aqui 0,0
				close(pipeServidor[1][0]);  //escreve aqui 1,1
			} else if (jogadorConectado == 2) {
				close(pipeServidor[0][0]);  //Escreve aqui 0,1
				close(pipeServidor[1][1]);  //Ouve aqui 1,0
			}
			//sai para evitar de os filhos chamarem o fork
			break;
		} else {
			printf("fork error\n");
			exit(1);
		}
	}

	if (pid == 0) {
		//código para os filhos
		char msgPlayer[200];
		char desenha_v[200];
		int rc, linha, col, win;
		if (jogadorConectado == 1) {
			while (1) {
				//Desenha e envia a velha para o primeiro jogador.
				desenha_velha(desenha_v);
				enviarMsg(desenha_v, playerSocket);
				//envia para o jogador 1 que é a  vez dele e aguarda ler a jogada
				//do while apenas para ler enquanto a posicao estiver incorreta
				do {
					enviarMsg("Sua vez! envie uma linha e coluna válida!>l c\n", playerSocket);
					rc = read(playerSocket, msgPlayer, sizeof(msgPlayer));
					//pega as coordenadas e marca a velha (obs, jogador 1 é X)
					linha = msgPlayer[0] - '0';
					col = msgPlayer[2] - '0';
				} while (marca_velha(linha, col, 'X') == -1);

				//envio a jogada para o outro servidor(filho) anotar a jogada recentemente feita
				enviarMsg(msgPlayer, pipeServidor[1][1]);

				//verifico se após a jogada ele ganhou
				if ((win = verifica_ganhador()) != 32) {
					break;
				}
				//espero o outro jogador realizar sua jogada
				enviarMsg("Aguarde sua vez...\n", playerSocket);
				rc = read(pipeServidor[0][0], msgPlayer, sizeof(msgPlayer));
				linha = msgPlayer[0] - '0';
				col = msgPlayer[2] - '0';
				//marco a jogada com O (obs segundo jogador é O)
				marca_velha(linha, col, 'O');
				if ((win = verifica_ganhador()) != 32) {
					break;
				}
			}
		} else if (jogadorConectado == 2) {
			while (1) {
				//aguarda receber do pipe do outro servidor a jogada
				enviarMsg("Aguarde sua vez...\n", playerSocket);
				rc = read(pipeServidor[1][0], msgPlayer, sizeof(msgPlayer));
				linha = msgPlayer[0] - '0';
				col = msgPlayer[2] - '0';
				//marco a jogada feita pelo 1 jogador. 1ºjogador é X
				marca_velha(linha, col, 'X');
				//verifico se ele ganhou
				if ((win = verifica_ganhador()) != 32) {
					break;
				}
				//desenho a velha e aguardo o jogador 2 realizar sua jogada
				desenha_velha(desenha_v);
				enviarMsg(desenha_v, playerSocket);
				do {
					enviarMsg("Sua vez! envie uma linha e coluna válida!>l c\n", playerSocket);
					rc = read(playerSocket, msgPlayer, sizeof(msgPlayer));
					linha = msgPlayer[0] - '0';
					col = msgPlayer[2] - '0';
				} while (marca_velha(linha, col, 'O') == -1);
				//marco a jogada e envio por pipe para o jogador 1
				enviarMsg(msgPlayer, pipeServidor[0][1]);
				//verificação se jogador 2 ganhou
				if ((win = verifica_ganhador()) != 32) {
					break;
				}
			}
		}
		//se alguem ganhou será relizado o break e aqui enviado as mensagens
		if (win == 0) {
			enviarMsg("\n\n*********Empatou*********\n\n", playerSocket);
		} else if (win == 'X') {
			if (jogadorConectado == 1) {
				enviarMsg("\n\nVocê Ganhou! Parabens!\n\n", playerSocket);
				desenha_velha(desenha_v);
				enviarMsg(desenha_v, playerSocket);
			} else {
				enviarMsg("\n\nVocê perdeu :[\n\n", playerSocket);
				desenha_velha(desenha_v);
				enviarMsg(desenha_v, playerSocket);
			}
		} else if (win == 'O') {
			if (jogadorConectado == 1) {
				enviarMsg("\n\nVocê perdeu :[\n\n", playerSocket);
				desenha_velha(desenha_v);
				enviarMsg(desenha_v, playerSocket);
			} else {
				enviarMsg("\n\nVocê Ganhou! Parabens!\n\n", playerSocket);
				desenha_velha(desenha_v);
				enviarMsg(desenha_v, playerSocket);
			}
		}
	} else if (pid) {
		/*Espera todos os filhos. O argumento -1 faz isso*/
		waitpid(-1, NULL, 0);
	}

}  // main
