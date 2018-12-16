#include <stdio.h>
#include <signal.h>
#include <unistd.h> 

/*imprime o sinal*/
void funcSignal(int signo){
    printf("Sinal enviado: %d\n", signo);
    fflush(stdout);
}

int main(){
    
    int i;
    /*realiza um for do 1 ao 64, que são os possiveis sinais*/
    for(i = 1; i < 65; i++){
        if(signal(i, funcSignal) == SIG_ERR){
            printf("\ncan't catch SIGKILL\n");
        }
    }
    

    while(1){
        sleep(1000);
    }
}