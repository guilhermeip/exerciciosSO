/* TODO: implement threaded code 
 To be done in class */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>

#define MAX_THREADS 2048



// Input vectors
float *a;
float *b;
// Output vector
float *c;

typedef struct vetores{
    int inicio;
    int fim;
}Vet;
 void *vecAdd(void * param){
    int i;
    Vet *estrutura = (Vet*) param;
    for(i = estrutura->inicio; i < estrutura->fim; i++){
        c[i] = a[i] + b[i];
    }    
}

int main( int argc, char *argv[] ){
    
    int num_threads, i;
    pthread_t tid[MAX_THREADS];     /* Vetor de Identificadores de threads. */
    pthread_attr_t attr;            /* atributos de thread. */

    if (argc != 3) {
        fprintf(stderr, "Uso:  a.out <numero de threads> <quantidade de elementos>\n");
        exit(0);
    }

    if (atoi(argv[1]) <= 0) {
        fprintf(stderr,"%d deve ser > 0\n", atoi(argv[1]));
        exit(0);
    }

    if (atoi(argv[1]) > MAX_THREADS) {
        fprintf(stderr,"%d must be <= %d\n", atoi(argv[1]), MAX_THREADS);
        exit(0);
    }

    if (atoi(argv[2]) <= 0) {
        fprintf(stderr,"%d deve ser > 0\n", atoi(argv[2]));
        exit(0);
    }
    
    // Size of vectors
    num_threads = atoi(argv[1]);
    printf("The number of threads is %d\n", num_threads);
    
    int n = atoi(argv[2]);
 
    // Size, in bytes, of each vector
    size_t bytes = n*sizeof(float);
 
    // Alocar os vetores.
    a = (float*)malloc(bytes);
    b = (float*)malloc(bytes);
    c = (float*)malloc(bytes);
 
    // Inicializa os vetores.
    for(i=0; i<n; i++) {
        a[i] = sinf(i)*sinf(i);
        b[i] = cosf(i)*cosf(i);
    }
    
    /* threads here!!!! */
    int fimIndex = (n/num_threads);

    Vet *vetores = malloc(num_threads* sizeof (Vet) );
    vetores[0].inicio = 0;
    vetores[0].fim = fimIndex + (n%num_threads);
    
    int fimThreadAnterior = vetores[0].fim;
    
    pthread_create(&tid[0], NULL, vecAdd, (void*)&vetores[0]);

    for(i=1; i < num_threads; i++){
        vetores[i].inicio = fimThreadAnterior;
        vetores[i].fim = vetores[i].inicio + fimIndex;
        fimThreadAnterior = vetores[i].fim;
        pthread_create(&tid[i], NULL, vecAdd, (void*)&vetores[i]);        
    }
    
    // Show results
    printf("Vetores: \n");
    for(i=0; i<n; i++){
        printf("%g + %g = %g\n", a[i], b[i], c[i]);
    }
 
    printf("fim.\n");
 
    // Release memory
    free(a);
    free(b);
    free(c);
    free(vetores);
 
    return 0;
}

