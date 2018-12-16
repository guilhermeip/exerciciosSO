#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX 10

void *cpuBound(void * param){
    int i, j, mult = 1;
    for(i = 0; i < 20000; i++){
        for(j = 0; j < 20000; j++){
            mult *= i*1;
        }
    }
}
void *ioBound(void * param){
    FILE *read = fopen("arqIn.txt", "r");
    FILE *write = fopen("arqOut.txt", "w");
    char c[1000]; 
    if (read == NULL || write == NULL){
        printf("Problemas ao ler arquivo\n");
        return;
    }
    int resultado;

    while(fgets(c, 1000, read) != NULL){
        fprintf(write, "%s", c);
    }

    fclose(read);
    fclose(write);
}
int main(){
    pthread_t tid[MAX];
    int i;
    for(i = 0; i < 5; i++){
        pthread_create(&tid[i], NULL, cpuBound, (void*)NULL);
    }
    for(i = 5; i < 10; i++){
        pthread_create(&tid[i], NULL, ioBound, (void*)&i);
    }

    for(i = 0; i < MAX; i++){
        pthread_join(tid[i], NULL);
    }
}