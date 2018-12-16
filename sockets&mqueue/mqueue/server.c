/*
Autor: Guilherme Ianhez Pereira dos Santos
data: 03/11/2018
*/
#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#define SERVER_QUEUE_NAME "/sp-example-serverr"
#define QUEUE_PERMISSIONS 0660
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 10

char pt[][100] = {"peixe", "bola", "gato"};

char en[][100] = {"fish", "ball", "cat"};

char jp[][100] = {"sakana", "tama", "neko"};

/*Encontra a palavra correspondente no vetor de
origem e retorna a palavra traduzida no vetor de destino.
Como as palavras em cada vetor estão na mesma ordem (pt-> peixe, bola, gato.
En->fish, ball, cat. encontra a palavra peixe (índice 1) e retorna en[1], ou
ERROR-UNKNOW, se não encontrar*/
char* encontrarPalavra(char ori[], char des[], char palavra[]) {
	int i = 0;
	char* retorno = malloc(10);
	printf("FUNC\nori: #%s#\ndes: #%s#\npalavra: #%s#\n", ori, des, palavra);
	if (strcmp(ori, "pt") == 0) {
		for (i = 0; i < 3; i++) {
			if (strcmp(palavra, pt[i]) == 0) {
				break;
			}
		}
	} else if (strcmp(ori, "en") == 0) {
		for (i = 0; i < 3; i++) {
			if (strcmp(palavra, en[i]) == 0) {
				break;
			}
		}
	} else if (strcmp(ori, "jp") == 0) {
		for (i = 0; i < 3; i++) {
			if (strcmp(palavra, jp[i]) == 0) {
				break;
			}
		}
	} else {
		return strcpy(retorno, "ERROR:UNKNOW");
	}

	if (strcmp(des, "pt") == 0) {
		if (i == 3) {
			return strcpy(retorno, "ERROR:UNKNOW");
		}
		return strcpy(retorno, pt[i]);

	} else if (strcmp(des, "en") == 0) {
		if (i == 3) {
			return strcpy(retorno, "ERROR:UNKNOW");
		}
		return strcpy(retorno, en[i]);
	} else if (strcmp(des, "jp") == 0) {
		if (i == 3) {
			return strcpy(retorno, "ERROR:UNKNOW");
		}
		return strcpy(retorno, jp[i]);
	} else {
		return strcpy(retorno, "ERROR:UNKNOW");
	}
}

int main(int argc, char** argv) {
	char palavra[MSG_BUFFER_SIZE];
	char ori[3];
	char des[3];
	mqd_t qd_server, qd_client;  // queue descriptors
	char in_buffer[MSG_BUFFER_SIZE];
	char out_buffer[MSG_BUFFER_SIZE];

	printf("\nServer\n");

	struct mq_attr attr;
	attr.mq_flags = 0;
	attr.mq_maxmsg = MAX_MESSAGES;
	attr.mq_msgsize = MAX_MSG_SIZE;
	attr.mq_curmsgs = 0;

	/*Cria a fila com as permissões*/
	if ((qd_server = mq_open(SERVER_QUEUE_NAME, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
		perror("Server: mq_open (server)\n");
		exit(1);
	} else
		printf("Server: Criado a fila de mensagem do servidor no modo somente leitura.\n");

	/*Recebe o nome da fila do cliente*/
	if (mq_receive(qd_server, in_buffer, MSG_BUFFER_SIZE, NULL) == -1) {
		perror("Server: mq_receive\n");
		exit(1);
	} else
		printf("Server: Recebido o nome da fila de mensagem criada pelo cliente.\n");

	//Abre a fila de mensagem do cliente
	if ((qd_client = mq_open(in_buffer, O_WRONLY)) == 1) {
		perror("Server: Not able to open client queue\n");
		exit(-1);
	} else
		printf("Server: Acesso à fila de mensagem do cliente estabelecida no modo somente escrita.\n");

	while (1) {
		// recebe a palavra a ser traduzida
		memset(in_buffer, 0, sizeof(in_buffer));
		if (mq_receive(qd_server, in_buffer, MSG_BUFFER_SIZE, NULL) == -1) {
			perror("Server: mq_receive\n");
			exit(1);
		}
		/*
        Separa cada elemento do protocolo. pt-en gato
        ori: pt
        des: en
        palavra: gato
        */
		strncpy(ori, in_buffer, 2);
		ori[2] = '\0';
		strncpy(des, in_buffer + 3, 2);
		des[2] = '\0';
		strcpy(palavra, in_buffer + 6);
		palavra[strlen(palavra) - 1] = '\0';

		printf("ori: #%s#\ndes: #%s#\npalavra: #%s#\nInbuf: #%s#\n", ori, des, palavra, in_buffer);

		/*Verifica se é para sair, com NO-NO (protocolo)*/
		if (strcmp(in_buffer, "NO-NO\n") == 0) {
			printf("\nSERVER: Recebido a mensagem de encerramento. Servidor encerrado\n");
			if (mq_close(qd_server) == -1) {
				perror("Client: mq_close");
				exit(1);
			}

			if (mq_unlink(SERVER_QUEUE_NAME) == -1) {
				perror("Client: mq_unlink");
				exit(1);
			}
			printf("Server: bye\n");
			exit(0);
		}

		char* palavra_buscada = encontrarPalavra(ori, des, palavra);
		printf("palavra enviada: #%s#", palavra_buscada);
		fflush(stdout);
		if (mq_send(qd_client, palavra_buscada, strlen(palavra_buscada) + 1, 0) == -1) {
			perror("Server: Not able to send message to client\n");
			continue;
		}
		free(palavra_buscada);
	}
}