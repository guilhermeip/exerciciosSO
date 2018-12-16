#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define MEMORY_SIZE (sizeof(shared_struct))

/*Estrtura que sera compartilhada*/
typedef struct shared_struct {
	int inteiro;
	float flutuante;
	char character;
	char string[30];
	long long_;
	double double_;
    short short_;
} shared_struct;

int main(){
	/*Essa função retorna uma chave unica que sera usada tanto para o "cliente" quanto para o "servidor"*/
    key_t key = ftok("shmfile",65); 

	/*Identificador da memoria compartilhada*/
    int shmid;

    shared_struct *shared_memory_address;
    
	/*Cria a região de memoria compartilhada*/
    if ( (shmid = shmget(key, MEMORY_SIZE, 0666)) < 0) {
		perror("Erro ao tentar criar o segmento de shm (shmget).");
		exit(1);
	}

	/*Se une ao segmento de memoria compartilhada, retornando o endereço da memoria que é compartilhada*/
    if ( (shared_memory_address = shmat(shmid, NULL, 0)) == (char*)-1 ) {
		perror("Erro ao acoplar o segmento ao espaço de dados do programa (shmat).");
		exit(1);
	}

	printf("Inteiro: %d\n", shared_memory_address->inteiro);
	printf("Flutuante: %f\n", shared_memory_address->flutuante);
	printf("Caracter: %c\n", shared_memory_address->character);
	printf("String: %s\n", shared_memory_address->string);
	printf("Long: %li\n", shared_memory_address->long_);
	printf("double: %f\n", shared_memory_address->double_);
	printf("short: %hi\n", shared_memory_address->short_);


    /*Saindo do segmento*/
    if (shmdt(shared_memory_address) == -1) {
		perror("Erro ao desacoplar a região de memória compartilhada (shmdt).");
		exit(1);
	}
    /* Destruição do segmento */
	if (shmctl(shmid, IPC_RMID, NULL) == -1){
		perror("Erro ao destruir o segmento compartilhado. (shmctl).") ;
		exit(1) ;
	}
}