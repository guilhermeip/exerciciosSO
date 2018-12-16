#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h> 


/*-------->COMENTARIOS IGUAIS AO Ex3_CLIENTE.c<---------*/

#define MEMORY_SIZE (sizeof(shared_struct))

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
    key_t key = ftok("shmfile",65); 
    int shmid;
    shared_struct *shared_memory_address;

    if((shmid = shmget(key, MEMORY_SIZE, 0666|IPC_CREAT)) == -1){
        perror("Erro ao tentar criar o segmento de shm (shmget).");
		exit(1);
    } 

    if ( (shared_memory_address = shmat(shmid, NULL, 0)) == (char*)-1 ) {
		perror("Erro ao acoplar o segmento ao espaço de dados do programa (shmat).");
		exit(1);
	}

    /*Saindo do segmento*/
    if (shmdt(inteiro) == -1) {
		perror("Erro ao desacoplar a região de memória compartilhada (shmdt).");
		exit(1);
	}
    /* Destruição do segmento */
	if (shmctl(shmid, IPC_RMID, NULL) == -1){
		perror("Erro ao destruir o segmento compartilhado. (shmctl).") ;
		exit(1) ;
	}

    shared_memory_address->inteiro = 32000;
    shared_memory_address->flutuante = 6.52;
    shared_memory_address->character = 'a';
    strncpy(shared_memory_address->string, "teste\0", 6);
    shared_memory_address->long_ = 2047483647;
    shared_memory_address->double_ = 4047483647;
    shared_memory_address->short_ = 32001;

    /* Saindo da região de memória compartilhada. */
	if (shmdt(shared_memory_address) == -1) {
		perror("Erro ao desacoplar a região de memória compartilhada (shmdt).");
		exit(1);
	}
	

}