/*
Autor: Guilherme Ianhez Pereira dos Santos
data: 03/11/2018
*/

#include <netdb.h>      /* dados sobre a rede (ex.: hosts, protocolos, ...) */
#include <netinet/in.h> /* fun��es, tipos e constantes para sockets (Internet) */
#include <sys/socket.h> /* fun��es, tipos e constantes para sockets (UNIX) */
#include <sys/types.h>  /* tipos b�sicos do GNU C */

#include <arpa/inet.h>
#include <stdio.h>  /* entrada/sa�da (I/O) */
#include <stdlib.h> /* exit() */
#include <string.h> /* manipula��o de strings */
#include <unistd.h> /* close() */

#define IP_SERVER "127.0.0.1" /* especifica o IP do servidor */
#define PORTA_PADRAO 8085     /* especifica a porta padrao de conexao */
#define MAX_BUFFER 300        /* tamanho maximo do buffer */

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
	struct hostent *ptrh;       /* ponteiro para a tabela de hosts */
	struct sockaddr_in address; /* estrutura para armazenar o IP e a porta */
	int sd;                     /* descritor do socket */
	int port;                   /* porta utilizada na comunica��o */
	char *host;                 /* armazena o endere�o do servidor */
	char buffer[MAX_BUFFER];    /* armazena os dados recebidos */

	/* verifica os par�metros passados por linha de comando */
	if (argc > 2)
		port = atoi(argv[2]); /* converte para inteiro */
	else
		port = PORTA_PADRAO; /* usa a porta padr�o */

	/* testa a validade da porta */
	if ((port <= 1024) || (port >= 65536)) {
		fprintf(stderr, "n�mero de porta inv�lido %s\n", argv[2]);
		exit(1);
	}  //if

	/* verifica se o endere�o do servidor foi fornecido */
	if (argc > 1)
		host = argv[1];
	else
		host = IP_SERVER;

	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\n Socket creation error \n");
		return -1;
	}

	memset(&address, '0', sizeof(address));

	address.sin_family = AF_INET;
	address.sin_port = htons(port);

	// Convert IPv4 and IPv6 addresses from text to binary form
	if (inet_pton(AF_INET, host, &address.sin_addr) <= 0) {
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}

	if (connect(sd, (struct sockaddr *)&address, sizeof(address)) < 0) {
		printf("\nConnection Failed \n");
		return -1;
	}

	int rc;
	while (1) {
		rc = read(sd, buffer, sizeof(buffer));
		buffer[rc] = '\0';
		printf("%s\n", buffer);
		fflush(stdout);
		if(strcmp(buffer, "") == 0){
			break;
		}
		if (strstr(buffer, "Sua vez!")) {
			rc = read(STDIN_FILENO, buffer, sizeof(buffer));
			buffer[rc] = '\0';
			enviarMsg(buffer, sd);
		}
	}  //while

	/* fecha a conex�o */
	close(sd);

	return 0;
}  // main
