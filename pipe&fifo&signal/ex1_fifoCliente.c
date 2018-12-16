#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>

/*------->COMENTARIOS IGUAIS AOS DO FIFO_SERVER<-------*/


/*Nome dos fifos para leitura e escrita, criados localmente*/
#define FIFOREAD "server2\0"
#define FIFOWRITE "server1\0"

/*Descritores de leitura e escrita*/
int file_descriptor_write, file_descriptor_read;

/*Função que trata o sinal SIGINT (ctrl+C), caso o usuario saia do programa, exclui os fifos criados*/
void sig_handler(int signo){
    char fifo_read[15];
    char fifo_write[15];
    sprintf(fifo_read, "rm -f %s", FIFOREAD);
    sprintf(fifo_write, "rm -f %s", FIFOWRITE);

    printf("\n##### CLOSING SECTION #####\n");
	close(file_descriptor_read);
	close(file_descriptor_write);
	system(fifo_read);
	system(fifo_write);
	printf("\n##### SECTION CLOSED #####\n");
	exit(0);
}
int main(void) {
	
	int readBytes;
	char buffer[256];
    char * fifo_read = FIFOREAD;
    char * fifo_write = FIFOWRITE;
    
    if (signal(SIGINT, sig_handler) == SIG_ERR)
        printf("\ncan't catch SIGKILL\n");
    
    if (mkfifo(fifo_write, 0666) == -1) {
		perror("##### MKFIFO WRITE #####\n");
	}else {
        printf("Servidor Write fifo criado com sucesso");
    }

	if ( (file_descriptor_write = open(fifo_write, O_WRONLY)) == -1 ){
		perror("##### OPEN WRITE #####\n");
		exit(-1);
	}else {
        printf("Servidor Write fifo ABERTO com sucesso\n");
    }

    if (mkfifo(fifo_read, 0666) == -1) {
		perror("##### MKFIFO READ #####\n");
	}else {
        printf("Servidor Read fifo criado com sucesso\n");
    }

    if ( (file_descriptor_read = open(fifo_read, O_RDONLY)) == -1 ){
		perror("##### OPEN READ #####\n");
		exit(-1);
	}else {
        printf("Servidor Read fifo ABERTO com sucesso\n");
    }
    
    while(1){
        read(file_descriptor_read, buffer, sizeof(buffer));
        fflush(stdout);
        memset(buffer, 0, sizeof(buffer));

        printf("Digite um número: ");
        scanf("%s", buffer);
        write(file_descriptor_write, buffer, sizeof(buffer));
        memset(buffer, 0, sizeof(buffer));

        read(file_descriptor_read, buffer, sizeof(buffer));
        fflush(stdout);
        printf("Pai responde: %s\n", buffer);
        memset(buffer, 0, sizeof(buffer));
    }
}