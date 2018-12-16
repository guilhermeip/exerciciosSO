#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define IN_USE 1
#define FREE 0


typedef enum philosopher_states { THINKING,
	                              HUNGRY,
	                              EATING } phil_states;

int n;  //numero de filosofos
pthread_mutex_t mutex;
pthread_cond_t *cond;
int *forks;

phil_states *philosopher;  // array de filosofos

void print_filosofos(void) {
	int i;
	for (i = 0; i < n; i++) {
		if (philosopher[i] == THINKING)
			printf(" T ");
		if (philosopher[i] == HUNGRY)
			printf(" H ");
		if (philosopher[i] == EATING)
			printf(" E ");
	}
	printf("\n");
}

void *pegar_garfos(int id) {
	int left = id;             // garfo da esquerda
	int right = (1 + id) % n;  // garfo da direita

	//Uma thread diz que vai realizar um lock para as demais (bloqueando-as)
	pthread_mutex_lock(&mutex);
	philosopher[id] = HUNGRY;
	print_filosofos();
	//Enquanto tiver garfos sendo utilizados na adjacencia a thread espera (wait)
	while (forks[right] == IN_USE || forks[left] == IN_USE) {
		pthread_cond_wait(&cond[id], &mutex);
	}
	//Depois que ambos os garfo forem liberados o filosofo pode comer
	philosopher[id] = EATING;
	forks[left] = IN_USE;
	forks[right] = IN_USE;

	pthread_mutex_unlock(&mutex);
	print_filosofos();
}

void *por_garfo(int id) {
	int left = id;             // garfo da esquerda
	int right = (1 + id) % n;  // garfo da direita

	pthread_mutex_lock(&mutex);
	forks[left] = FREE;
	forks[right] = FREE;
	print_filosofos();
	philosopher[id] = THINKING;
	/*Acordando os filosofos da esquerda e direita*/
	pthread_cond_signal(&cond[left]);
	pthread_cond_signal(&cond[right]);
	pthread_mutex_unlock(&mutex);
}

void pensando() {
	sleep((rand() % 10 + 1));
}

void comendo() {
	sleep((rand() % 10 + 1));
}

void *acao_filosofo(void *arg) {
	int i = (int)arg;
	while (1) {
		pensando();
		pegar_garfos(i);
		comendo();
		por_garfo(i);
	}
	pthread_exit(0);
}

int main(int argc, const char *argv[]) {
	pthread_t *threads;
	int i;

	if (argc != 2) {
		printf("Necessário passar por parâmetro o número de filosofos (threads) que deseja!\n");
		exit(1);
	}

	//numero de filosofos
	n = atoi(argv[1]);

	printf("**** %d THREADS **** \n", n);
	/* Inicializações */
	threads = (pthread_t *)malloc(n * sizeof(pthread_t *));
	philosopher = (phil_states *)malloc(n * sizeof(phil_states));
	cond = (pthread_cond_t *)malloc(n * sizeof(pthread_cond_t));
	forks = (int *)malloc(n * sizeof(int *));
	pthread_mutex_init(&mutex, NULL);

	//a principio todos os filosofos estao pensando e agora no monitor temos um vetor de garfos
	//disponiveis
	for (i = 0; i < n; i++) {
		philosopher[i] = THINKING;
		forks[i] = FREE;
	}


	//cria as threads e inicia as condições também
	for (i = 0; i < n; i++) {
		pthread_cond_init(&cond[i], NULL);
		pthread_create(&threads[i], NULL, acao_filosofo, (void *)(i));
	}

	pthread_exit(0);
	return 0;
}