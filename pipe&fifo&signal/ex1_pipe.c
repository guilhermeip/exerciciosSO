#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/*
Programa desenvolvido por: Guilherme Ianhez Pereira dos Santos
O filho envia um numero que deseja saber a raiz quadrada e o pai responde o numero correspondende

Compilador utilizado: clang
Argumentos para o compilador: -lm (argumento da biblioteca math.h)
*/

/* Programa principal */
int main(void) {
	pid_t pid;

	/*Contém 2 pipes, pois o filho envia dados e recebe do pai e o pai envia e recebe também*/
	int mypipeA[2];
	int mypipeB[2];

	/* Criar o pipeA. */
	if (pipe(mypipeA)) {
		fprintf(stderr, "Falha ao criar o Pipe.\n");
		return EXIT_FAILURE;
	}
	/* Criar o pipeB. */
	if (pipe(mypipeB)) {
		fprintf(stderr, "Falha ao criar o Pipe.\n");
		return EXIT_FAILURE;
	}

	/* Criar o processo filho. */
	pid = fork();
	if (pid < (pid_t) 0) {
        /* pid negativo, falha no fork. */
        fprintf(stderr, "Falha ao executar o Fork.\n");
        return EXIT_FAILURE;
	} else if (pid == (pid_t) 0) {
        /* No processo filho. */
		char buffer[50];

		/*Irá escutar no mypipeA e escrever no mypipeB*/
		close(mypipeA[1]);
		close(mypipeB[0]);

		while(1){
			/*Recebe a sempre a frase do pai: Qual a raiz quadrada você deseja descobrir?*/
			read(mypipeA[0], buffer, sizeof(buffer));
			fflush(stdout);
			printf("Pai disse: **%s**\n", buffer);
			/*zera o buffer*/
			memset(buffer, 0, strlen(buffer));

			/*Envia o numero para o pai*/
			printf("Filho: Digite um numero: ");
			scanf("%s", buffer);
			write(mypipeB[1], buffer, sizeof(buffer));
			memset(buffer, 0, strlen(buffer));
			
			/*recebe a responsta*/
			read(mypipeA[0], buffer, sizeof(buffer));
			fflush(stdout);
			printf("Pai disse: %s\n", buffer);
			memset(buffer, 0, strlen(buffer));
		}

	} else {
		/* Processo pai. */
    	char buffer[50];

		/*Irá escutar no mypipeB e escrever no mypipeA*/
		close(mypipeB[1]);
		close(mypipeA[0]);
		
		while(1){
			/*Envia sempre essa pergunta para o filho*/
			sprintf(buffer, "Qual a raiz quadrada você deseja descobrir?");
			write(mypipeA[1], buffer, sizeof(buffer));
			memset(buffer, 0, strlen(buffer));

			/*escuta o numero que o filho deseja saber a resposta*/
			read(mypipeB[0], buffer, sizeof(buffer));
			fflush(stdout);
			printf("FILHO disse: ...#%s#\n", buffer);

			int num = atoi(buffer);
			memset(buffer, 0, strlen(buffer));

			/*Envia a resposta*/
			sprintf(buffer, "%.2f", sqrt(num));
			write(mypipeA[1], buffer, sizeof(buffer));
			memset(buffer, 0, strlen(buffer));
		}
	}
}