/*
Autor: Guilherme Ianhez Pereira dos Santos
data: 03/11/2018
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

char* socket_path = "\0hidden";

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
		;
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
		return "ERROR:UNKOWN";
	}
}

int main(int argc, char* argv[]) {
	struct sockaddr_un addr;
	char buf[100];
	int fd, cl, rc;

	if (argc > 1)
		socket_path = argv[1];

	/*Cria o socket com fluxo constante, SOCK_STREAM*/
	if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("socket error");
		exit(-1);
	}

	memset(&addr, 0, sizeof(addr));
	/*Seta para a familia Unix*/
	addr.sun_family = AF_UNIX;

	if (*socket_path == '\0') {
		*addr.sun_path = '\0';
		strncpy(addr.sun_path + 1, socket_path + 1, sizeof(addr.sun_path) - 2);
	} else {
		strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);
		// Exclui o socket antigo.
		unlink(socket_path);
	}

	/*Liga o socket criado com uma ip, ou caminho (caso Unix)*/
	if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		perror("bind error");
		exit(-1);
	}

	/*soquete do servidor em um modo passivo,
  onde ele aguarda o cliente se aproximar do servidor para fazer uma conexão.*/
	if (listen(fd, 5) == -1) {
		perror("listen error");
		exit(-1);
	}

	/*extrai a primeira solicitação de conexão na fila de conexões pendentes
  para o soquete de escuta, retorna um file descriptor do cliente*/
	if ((cl = accept(fd, NULL, NULL)) == -1)
		perror("accept error");

	while (1) {
		/*Lê do cliente e salva em buf*/
        memset(buf, 0, sizeof(buf));

		rc = read(cl, buf, sizeof(buf));
		buf[rc] = '\0';
		printf("read %u bytes: %.*s\n", rc, rc, buf);

		if (rc == -1) {
			perror("read");
			exit(-1);
		} else if (rc == 0) {
			printf("EOF\n");
			close(cl);
		}

		char palavra[10];
		char* palavra_buscada;
		char ori[3];
		char des[3];
		/*Separa cada elemento do protocolo. pt-en gato
    ori: pt
    des: en
    palavra: gato*/
		strncpy(ori, buf, 2);
		ori[2] = '\0';
		strncpy(des, buf + 3, 2);
		des[2] = '\0';
		strcpy(palavra, buf + 6);
		palavra[strlen(palavra) - 1] = '\0';

		/*Verifica se é para sair, com NO-NO (protocolo)*/
		if (strcmp(buf, "NO-NO\n") == 0) {
			printf("\nServidor encerrado.\n");
			close(cl);
			close(fd);
			return 0;
		}

		printf("word:%s#\n", palavra);
		printf("ori:%s#\n", ori);
		printf("des:%s#\n", des);

		/*Pega a palavra correspondente(traduzida)*/
		palavra_buscada = encontrarPalavra(ori, des, palavra);
		memset(palavra, 0, sizeof(palavra));
		rc = strlen(palavra_buscada);
		palavra_buscada[rc] = '\0';

		/*Escreve a palavra traduzida para o cliente*/
		if (write(cl, palavra_buscada, rc) != rc) {
			if (rc > 0)
				fprintf(stderr, "partial write");
			else {
				perror("write error");
				exit(-1);
			}
		}
		free(palavra_buscada);
	}

	return 0;
}