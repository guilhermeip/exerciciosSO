#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h>
#include <string.h> 
#include <signal.h> 
#include <sys/types.h>
#include <stdlib.h> 
#include <unistd.h> 
#include <math.h>

/*Nome dos fifos para leitura e escrita, criados localmente*/
#define FIFOREAD "server1\0"
#define FIFOWRITE "server2\0"

/*Descritores de leitura e escrita*/
int file_descriptor_read, file_descriptor_write;

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

int main(){
    char * fifo_read = FIFOREAD;
    char * fifo_write = FIFOWRITE;
    char buffer[256];
    
    int num;

    /*Define a sig_handler como a função para tratar sinais SIGINT*/
    if (signal(SIGINT, sig_handler) == SIG_ERR)
        printf("\ncan't catch SIGKILL\n");
    
    /*Cria a fifo de leitura*/
    if (mkfifo(fifo_read, 0666) == -1) {
		perror("##### MKFIFO READ #####\n");
	}else {
        printf("Servidor Read fifo criado com sucesso\n");
    }

    /*Abre a fifo de leitura*/
    if ( (file_descriptor_read = open(fifo_read, O_RDONLY)) == -1 ){
		perror("##### OPEN READ #####\n");
		exit(-1);
	}else {
        printf("Servidor Read fifo ABERTO com sucesso\n");
    }

    /*Cria a fifo de escrita*/
    if (mkfifo(fifo_write, 0666) == -1) {
		perror("##### MKFIFO WRITE #####\n");
	}else {
        printf("Servidor Write fifo criado com sucesso\n");
    }

    /*Abre a fifo de escrita*/
	if ( (file_descriptor_write = open(fifo_write, O_WRONLY)) == -1 ){
		perror("##### OPEN WRITE #####\n");
		exit(-1);
	}else {
        printf("Servidor Write fifo ABERTO com sucesso\n");
    }

    fflush(stdout);

    while(1){
        /*Identico ao programa da fifo, onde pede a raiz que o cliente deseja saber e responde
        com a raiz*/
        sprintf(buffer, "Qual raiz quadrada deseja saber?");
        write(file_descriptor_write, buffer, sizeof(buffer));
        memset(buffer, 0, sizeof(buffer));
        
        read(file_descriptor_read, buffer, sizeof(buffer));
        fflush(stdout);
        printf("Filho disse: #%s#\n", buffer);
        num = atoi(buffer);
        sprintf(buffer, "%.2f", sqrt(num));
        
        write(file_descriptor_write, buffer, sizeof(buffer));
        memset(buffer, 0, sizeof(buffer));
        
    }

}