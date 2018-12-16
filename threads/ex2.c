#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int *somaLinha;

typedef struct controle{
    int **matriz;
    int comeca;
    int termina;
    int N;
}Control;

void *somaMatriz(void *param){
    Control *c = (Control*) param;
    int i, j;
    int soma = 0;
    for(i = c->comeca; i < c->termina; i++){
        for(j = 0; j < c->N; j++){
            soma += c->matriz[i][j];
        }
        somaLinha[i] = soma;
        soma = 0;
    }
}

int main(int argc, char *argv[]){

    if (argc != 4) {
        fprintf(stderr, "Uso:  a.out <numThreads> <M> <N>\n");
        exit(0);
    }


    if (atoi(argv[1]) <= 0) {
        fprintf(stderr,"%d deve ser > 0\n", atoi(argv[1]));
        exit(0);
    }

    if (atoi(argv[2]) <= 0) {
        fprintf(stderr,"%d deve ser > 0\n", atoi(argv[1]));
        exit(0);
    }

    if (atoi(argv[3]) <= 0) {
        fprintf(stderr,"%d deve ser > 0\n", atoi(argv[1]));
        exit(0);
    }

    clock_t begin = clock(); // timer
    srand((unsigned) time(NULL)); // seed random
    int numThreads = atoi(argv[1]);
    int M = atoi(argv[2]);
    int N = atoi(argv[3]);

    //alocando o vetor da soma de cada linha
    somaLinha = malloc(M*sizeof(int));

    pthread_t tid[numThreads];
    int i,j;

    int **matriz = malloc(M*sizeof(int*));

    for(i = 0; i < M; i++){
        matriz[i] = malloc(N*sizeof(int));
        for(j = 0; j < N; j++){
            matriz[i][j] = (rand()) % 1000;
        }
    }
    
    Control *controle = malloc(sizeof(Control)* M);

    int fimIndex = (M/numThreads);

    controle[0].matriz = matriz;
    controle[0].N = N;
    controle[0].comeca = 0;
    controle[0].termina = fimIndex + (M%numThreads);

    int fimThreadAnterior = controle[0].termina;
    pthread_create(&tid[0], NULL, somaMatriz, (void*)&controle[0]);

    for(i = 1; i < numThreads; i++){
        controle[i].matriz = matriz;
        controle[i].N = N;
        controle[i].comeca = fimThreadAnterior;
        controle[i].termina = fimThreadAnterior + fimIndex;
        fimThreadAnterior = controle[i].termina;
        pthread_create(&tid[i], NULL, somaMatriz, (void*)&controle[i]);
    }

    for(i = 0; i < numThreads; i++){
        pthread_join(tid[i], NULL);
    }
    //resultado
    // for(i = 0; i < M; i++){
    //     for(j = 0; j < N; j++){
    //         printf("%d ", matriz[i][j]);
    //     }
    //     printf("= %d\n", somaLinha[i]);
    // }
    for(i = 0; i < M; i++){
        free(matriz[i]);
    }
    free(matriz);
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

    printf("Tempo deu %f com %d Thread(s)", time_spent, numThreads);
}