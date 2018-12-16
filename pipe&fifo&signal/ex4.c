#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h>
#include <stdlib.h> 
#include <unistd.h> 
#include <math.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <time.h>

/*
Programa desenvolvido por: Guilherme Ianhez Pereira dos Santos
Visa compreender as técnicas de comunicação entre processos (IPC)
Pipe e memória compartilhada em C

Para compilar: Utilize o argumento -lm que faz parta da biblioteca Math.h
Testado com compilador clang
*/

#define MEMORY_SIZE sizeof(float)

typedef struct TAM{
    int inicio;
    int fim;
}Tam;

void vecAdd(Tam estrutura, float *a, float *b, float *c){
    int i;
    for(i = estrutura.inicio; i < estrutura.fim; i++){
        c[i] = a[i] + b[i];
    }    
}
int main(int argc, char *argv[]){
    clock_t begin = clock(); // timer (començando o tempo)

    /*Espera 2 argumentos, nº processos filhos e quantidade de elementos
    Ex: ./ex4 4 10*/
    if (argc != 3) {
        fprintf(stderr, "Uso:  a.out <numero de prcessos-Filho> <quantidade de elementos>\n");
        exit(0);
    }

    if (atoi(argv[1]) <= 0) {
        fprintf(stderr,"%d deve ser > 0\n", atoi(argv[1]));
        exit(0);
    }

    if (atoi(argv[2]) <= 0) {
        fprintf(stderr,"%d deve ser > 0\n", atoi(argv[2]));
        exit(0);
    }
    /*Declarações de variaveis*/
    int i;
    pid_t pid;

    /*Numeros de processos-filhos*/
    int n = atoi(argv[1]);

    /*quantidade de elementos do vetor que sera compartilhado entre os processos*/
    int quantElementos = atoi(argv[2]);

    /*Cada processo filho tera um pipe para comunicação com pai*/
    int mypipeA[n][2];

    /*Variaveis para compartilhamento de memoria (ftok-> uma chave unica para esse conjunto de string e numero
    shmid é o file descriptor da memoria compartilhada*/
    key_t key = ftok("shmfile",65); 
    int shmid;

    /*Variaveis que serão passadas via pipe. Tam é uma struct que contem os valores de inicio
    e fim do vetor que cada processo-filho terá que trabalhar (realizar somas)*/
    Tam vetores[n];

    /*Dividindo corretamente o vetor para cada processo filho trabalhar:
    Ex: quantidade de elementos = 10; quantidade de processos = 3.
    10/3 = 3.333(...)
    Primeiro processo fará 4 e o restante 3, totalizando 10*/
    int fimIndex = (quantElementos/n); // 10/3 = 3
    vetores[0].inicio = 0;
    vetores[0].fim = fimIndex + (quantElementos%n); // 3 + 1 = 4, ou seja ((int) (10/3) + (10%3) = 4)
    int fimThreadAnterior = vetores[0].fim;

    // Input vectors
    float a[quantElementos];
    float b[quantElementos];

    for(i=0; i<quantElementos; i++) {
        a[i] = sinf(i)*sinf(i);
        b[i] = cosf(i)*cosf(i);
    }

    for (i = 0; i < n; ++i) {
        /* Criar o pipe. */
        if (pipe(mypipeA[i])) {
            fprintf(stderr, "Falha ao criar o Pipe.\n");
            return EXIT_FAILURE;
        }
    }

    /* Start children. */
    for (i = 0; i < n; ++i) {
        pid = fork();
        
        if (pid > 0) {   /* Sou pai, criarei mais filhos */
            close(mypipeA[i][0]);
            /*Como ja foi criado a primeira posição vetores[0], esse if serve para passar
            as proximas posições*/
            if(i >= 1){
                vetores[i].inicio = fimThreadAnterior;
                vetores[i].fim = vetores[i].inicio + fimIndex;
                fimThreadAnterior = vetores[i].fim;
            }
            /*Manda para o filho esse estrutuda contendo inicio e fim*/
            write(mypipeA[i][1], &vetores[i], sizeof(Tam));

            continue;
        } else if (pid == 0) { /* Sou filho, vou trabalhar */
            close(mypipeA[i][1]);
            /*Para o laço para que o filho não crie novos forks*/
            break;  
        } else {
            printf("fork error\n");
            exit(1);
        }
    }
    if(pid == 0){
        /*irá receber a memoria compartilhada (um vetor de float)*/
        float *c;
        
        Tam vet_filho;
        read(mypipeA[i][0], &vet_filho, sizeof(Tam));
        fflush(stdout);

        /*Cria a região compartilhada*/
        if((shmid = shmget(key, MEMORY_SIZE*quantElementos, 0666|IPC_CREAT)) == -1){
            perror("Erro1 ao tentar criar o segmento de shm (shmget).");
            exit(1);
        } 

        /*Conecta a região de memoria compartilhada. (retorna o endereço da memoria compartilhada)*/
        if ( (c = shmat(shmid, NULL, 0)) == (char*)-1 ) {
            perror("Erro ao acoplar o segmento ao espaço de dados do programa (shmat).");
            exit(1);
        }
        /*funcao que fará a soma*/
        vecAdd(vet_filho, a, b, c);

        /* Saindo da região de memória compartilhada. */
        if (shmdt(c) == -1) {
            perror("Erro ao desacoplar a região de memória compartilhada (shmdt).");
            exit(1);
        }
    }else{
        /*irá receber a memoria compartilhada (um vetor de float)*/
        float *c;

        /*Cria a região compartilhada*/
        if((shmid = shmget(key, MEMORY_SIZE*quantElementos, 0666|IPC_CREAT)) == -1){
            perror("Erro ao tentar criar o segmento de shm (shmget).");
            exit(1);
        } 

        /*Conecta a região de memoria compartilhada. (retorna o endereço da memoria compartilhada)*/
        if ( (c = shmat(shmid, NULL, 0)) == (char*)-1 ) {
            perror("Erro ao acoplar o segmento ao espaço de dados do programa (shmat).");
            exit(1);
        }

        /*Espera todos os filhos. O argumento -1 faz isso*/
        waitpid(-1, NULL, 0);

        /*printando o resultado*/
        for(i = 0; i < quantElementos; i++){
            printf("%g + %g = %g\n", a[i], b[i], c[i]);
        }
        
        /* Saindo da região de memória compartilhada. */
        if (shmdt(c) == -1) {
            perror("Erro ao desacoplar a região de memória compartilhada (shmdt).");
            exit(1);
        }
        
        /* Destruição do segmento apos termino dos filhos*/
        if (shmctl(shmid, IPC_RMID, NULL) == -1){
            perror("Erro ao destruir o segmento compartilhado. (shmctl).") ;
            exit(1) ;
        }

        /*Finalizando o tempo*/
        clock_t end = clock();
        double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

        printf("Tempo deu %f com o pai(s)", time_spent);
    }
}