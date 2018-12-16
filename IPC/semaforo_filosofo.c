#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef enum philosopher_states { THINKING,
	                              HUNGRY,
	                              EATING } filosofo_estado;

int N;
filosofo_estado *filosofos;
//semaforos
sem_t mutex;
sem_t *sem_fil;

//funcao que mostra o estado dos N filosofos
void print_filosofos() {
	int i;
	for (i = 0; i < N; i++) {
		if (filosofos[i] == THINKING)
			printf(" T ");
		if (filosofos[i] == HUNGRY)
			printf(" H ");
		if (filosofos[i] == EATING)
			printf(" E ");
	}
	printf("\n");
}

//funcao que testa se o filosofo pode comer
//No caso se o filosofo da esquerda e da direita nao estiverem comendo, ele pega os garfos
void test(int i) {
	int LEFT = (i + N - 1) % N;
	int RIGHT = (i + 1) % N;
	if (filosofos[i] == HUNGRY && filosofos[LEFT] != EATING && filosofos[RIGHT] != EATING) {
		filosofos[i] = EATING;
		print_filosofos();
		sem_post(&sem_fil[i]);
	}
}

//para pegar o garfo é necessário o mutex, evitando que mais de um filosofo fique com fome e consiga
//pegar mais de um 2 garfos.
//Caso um filosofo esteja comendo ou na direita ou ne esquerda, o sem_fil deixa esse filosofo esperando até liberar
void pegar_garfo(int i) {
	sem_wait(&mutex);
	filosofos[i] = HUNGRY;
	print_filosofos();
	test(i);
	sem_post(&mutex);
	sem_wait(&sem_fil[i]);
}


//A liberação do garfo acontece primeiro na esquerda, então caso 2 filosofos estejam esperando
//para comer na sua adjacencia, o primeiro a ser liberado para comer será o da esquerda
void por_garfo(int i) {
	sem_wait(&mutex);
	int LEFT = (i + N - 1) % N;
	int RIGHT = (i + 1) % N;
	filosofos[i] = THINKING;
	print_filosofos();
	test(LEFT);
	test(RIGHT);
	sem_post(&mutex);
}

//a thread(filosofo) espera um tempo aleatoria pensando
void pensando() {
	sleep((rand() % 10 + 1));
}

//a thread(filosofo) espera um tempo aleatorio comendo
void comendo() {
	sleep((rand() % 10 + 1));
}
//acao do filosofo
void *acao_filosofo(void *j) {
	int i = *(int *)j;
	while (1) {
		pensando();
		pegar_garfo(i);
		comendo();
		por_garfo(i);
	}
}

int main(int argc, const char *argv[]) {
	if (argc != 2) {
		printf("Necessário passar por parâmetro o número de filosofos (threads) que deseja!\n");
		exit(1);
	}

	//N seria o numero de filosofos
	N = atoi(argv[1]);
	filosofos = (filosofo_estado *)malloc(N * sizeof(filosofo_estado));
	sem_fil = (sem_t *)malloc(N * sizeof(sem_t));

	int i;
	int resultado_chamada_Thread_Sem;

	//A principio todos os filosofos estão pensando
	for (i = 0; i < N; i++) {
		filosofos[i] = THINKING;
	}
    printf("**** %d THREADS(filosofos) **** \n", N);
	print_filosofos();

	pthread_t thread[N];

	void *thread_result;

	//inicia os semaforos

	resultado_chamada_Thread_Sem = sem_init(&mutex, 0, 1);
	if (resultado_chamada_Thread_Sem != 0) {
		perror("Erro na inicialização do semaforo!");
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < N; i++) {
		resultado_chamada_Thread_Sem = sem_init(&sem_fil[i], 0, 0);
		if (resultado_chamada_Thread_Sem != 0) {
			perror("Erro na inicialização do semaforo!");
			exit(EXIT_FAILURE);
		}
	}

	//cria as threads(filosofos)

	for (i = 0; i < N; i++) {
		resultado_chamada_Thread_Sem = pthread_create(&thread[i], NULL, acao_filosofo, &i);
		if (resultado_chamada_Thread_Sem != 0) {
			perror("Erro na inicialização da thread!");
			exit(EXIT_FAILURE);
		}
	}
	//faz um join nas threads, não deixando a main acabar e fechar todas as threads dos filosofos
	for (i = 0; i < N; i++) {
		resultado_chamada_Thread_Sem = pthread_join(thread[i], &thread_result);
		if (resultado_chamada_Thread_Sem != 0) {
			perror("Erro ao fazer join nas threads!");
			exit(EXIT_FAILURE);
		}
	}

	return 0;
}
