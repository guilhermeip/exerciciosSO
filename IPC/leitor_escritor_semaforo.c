#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_ITEM 10
#define PRODUTOR 0
#define CONSUMIDOR 1
sem_t mutex;
sem_t sem_write;
int quantidade_readers = 0;

//Funcao para esperar um tempo aleatório (simula a leitura e a escrita)
void wait() {
	sleep((rand() % 10 + 1));
}

//Um semaforo para escrever binario, deixando um unico escritor participar por vez
void *escrever(void *arg) {
    while(1){
        wait();
        sem_wait(&sem_write);
        printf("Começando a escrita\n"); 
        printf("Terminando a escrita\n");
        sem_post(&sem_write);
    }
}

//comeca a leitura, nesse caso é necessário um mutex no leitores para realizar a verificação do if
//e incrementar a quantidade de reads, caso o readers for == 0 bloqueia para os escritores.
//Mas permite que mais pessoas leiam
void start_read(){
    sem_wait(&mutex);
    if(quantidade_readers == 0){
        sem_wait(&sem_write);
    }
    quantidade_readers++;
    sem_post(&mutex);
    printf("Estou lendo\n");
    wait();
}

//Quando acaba a leitura verifica se nao existem mais leitores, nesse caso libera para os escritores

void end_read(){
    printf("Acabei de ler\n");
    sem_wait(&mutex);
    quantidade_readers--;
    if(quantidade_readers <= 0){
        sem_post(&sem_write);
    }
    sem_post(&mutex);
}
void *ler(void * arg) {
    while(1){
        wait();
        start_read();
        end_read();
    }
}   
int main(int argc, char *argv[]){
    if(argc != 3){
        printf("Necessário 2 argumentos, número de escritores e número de leitores");
        return -1;
    }

    int return_thread;
    int num_escritor = atoi(argv[1]);
    int num_leitores = atoi(argv[2]);
    pthread_t thread_escritor[num_escritor];
    pthread_t thread_leitor[num_leitores];

    if(sem_init(&mutex, 0, 1) != 0){
        perror("Erro na inicialização do semaforo!");
		exit(EXIT_FAILURE);
    }
    if(sem_init(&sem_write, 0, 1) != 0){
        perror("Erro na inicialização do semaforo!");
		exit(EXIT_FAILURE);
    }
    int i;
    for(i = 0; i < num_escritor; i++){
        return_thread = pthread_create(&thread_escritor[i], NULL, escrever, (void*)&i);
        if (return_thread != 0) {
			perror("Erro na inicialização da thread!");
			return -1;
		}
    }
    for(i = 0; i < num_leitores; i++){
        return_thread = pthread_create(&thread_leitor[i], NULL, ler, (void*)&i);
        if (return_thread != 0) {
			perror("Erro na inicialização da thread!");
			return -1;
		}
    } 
    void *thread_result;
    //faz um join nas threads, não deixando a main acabar e fechar todas as threads dos filosofos
	for (i = 0; i < num_leitores; i++) {
		return_thread = pthread_join(thread_leitor[i], &thread_result);
		if (return_thread != 0) {
			perror("Erro ao fazer join nas threads!");
			exit(EXIT_FAILURE);
		}
	}
    return 0;
}