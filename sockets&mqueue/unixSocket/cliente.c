/*
Autor: Guilherme Ianhez Pereira dos Santos
data: 03/11/2018
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

char *socket_path = "\0hidden";

int main(int argc, char *argv[]) {
	struct sockaddr_un addr;
	char buf[100];
	int fd, rc;

	if (argc > 1) socket_path = argv[1];

	// Instancia o socket do tipo UNIX com o protocolo de transmissão Stream
	if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("socket error");
		exit(-1);
	}

	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	if (*socket_path == '\0') {
		*addr.sun_path = '\0';
		strncpy(addr.sun_path + 1, socket_path + 1, sizeof(addr.sun_path) - 2);
	} else {
		strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);
	}

	/*Conecta no servidor com o ip ou caminho (unix) especificado*/
	if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
		perror("connect error");
		exit(-1);
	}

	while (1) {
		/*Lê do teclado*/
		rc = read(STDIN_FILENO, buf, sizeof(buf));
		buf[rc] = '\0';

		/*Escreve para o servidor*/
		if (write(fd, buf, rc) != rc) {
			if (rc > 0)
				fprintf(stderr, "partial write");
			else {
				perror("write error");
				exit(-1);
			}
		}
		/*Verifica se é para sair com NO-NO (protocolo)*/
		if (strcmp(buf, "NO-NO\n") == 0) {
			close(fd);
			return 0;
		}

		/*Recebe e exibe a tradução do servidor*/
		rc = read(fd, buf, sizeof(buf));
		buf[rc] = '\0';
		printf("traducao: %s\n", buf);

		if (rc == -1) {
			perror("read");
			exit(-1);
		} else if (rc == 0) {
			printf("EOF\n");
			close(fd);
		}
	}

	return 0;
}
