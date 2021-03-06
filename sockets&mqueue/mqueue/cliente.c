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
#include <unistd.h>

#define SERVER_QUEUE_NAME "/sp-example-serverr"
#define QUEUE_PERMISSIONS 0660
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 10

int main(int argc, char** argv) {
	char client_queue_name[64];
	int rc;
	mqd_t qd_server, qd_client;  // queue descriptors

	// Nome da fila do cliente
	sprintf(client_queue_name, "/cliente");

	struct mq_attr attr;
	attr.mq_flags = 0;
	attr.mq_maxmsg = MAX_MESSAGES;
	attr.mq_msgsize = MAX_MSG_SIZE;
	attr.mq_curmsgs = 0;

	/*Cria a fila do cliente*/
	if ((qd_client = mq_open(client_queue_name, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
		perror("Client: mq_open (client)");
		exit(1);
	}

	/*Se conecta com o server*/
	if ((qd_server = mq_open(SERVER_QUEUE_NAME, O_WRONLY)) == -1) {
		perror("Client: mq_open (server)");
		exit(1);
	}

	char in_buffer[MSG_BUFFER_SIZE];
	char temp_buf[256];

	/*Manda o nome da fila do cliente para o servidor abri-lá*/
	if (mq_send(qd_server, client_queue_name, strlen(client_queue_name) + 1, 0) == -1) {
		perror("Client: Not able to send message to server");
		exit(-1);
	}

	while (1) {
		/*le do teclado*/
		fflush(stdin);
		rc = read(STDIN_FILENO, temp_buf, sizeof(temp_buf));
		temp_buf[rc] = '\0';
		//envia para o servidor a palavra a ser traduzida
		if (mq_send(qd_server, temp_buf, rc, 0) == -1) {
			perror("Client: Not able to send message to server");
			continue;
		}
		//verifica se é para sair
		if (strcmp(temp_buf, "NO-NO\n") == 0) {
			printf("Client: Cliente encerrado.\n");
			if (mq_close(qd_client) == -1) {
				perror("Client: mq_close");
				exit(1);
			}

			if (mq_unlink(client_queue_name) == -1) {
				perror("Client: mq_unlink");
				exit(1);
			}
			printf("Client: bye\n");

			exit(0);
		}

		//recebe a tradução
		if (mq_receive(qd_client, in_buffer, MSG_BUFFER_SIZE, 0) == -1) {
			perror("Client: mq_receive");
			exit(1);
		}
		// Printa a tradução
		printf("\nClient: Tradução recebida: %s\n\n", in_buffer);
		fflush(stdout);
	}
}
