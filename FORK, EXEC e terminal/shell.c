#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define KEYBORD_TAM 1024
#define ARGS_TAM 64

typedef struct HISTORICO {
  char** comands;
} History;

History* h;
int i = 0;

int getTam(History* h) {
  int i = 0;
  while (h->comands[i] != NULL) {
    i++;
  }
  return i;
}

History* initHistory(int tam) {
  History* h = malloc(sizeof(History));
  h->comands = malloc(sizeof(char*) * tam);
  return h;
}

//Lê a linha como um todo ex: #ls -a#
	//Lê de acordo com a demanda do comando, se for maior que 1024 caracteres coloca + 1024 caracteres
	//e continua a leitura
char* readLine(void) {
  int bufsize = KEYBORD_TAM;
  char* linha = malloc(bufsize);
  char c;
  int i = 0;

  do {
    c = getchar();
    linha[i] = c;
    i++;
    if (i >= bufsize) {
      bufsize += KEYBORD_TAM;
      linha = realloc(linha, bufsize);
    }
  } while (c != EOF && c != '\n');
  linha[i - 1] = '\0';

  return linha;
}

//Quebra tudo que estava em uma linha em varios vetores de string. Ex: #ls# #-a#
char** readArgs(char* linha) {
  int bufsize = ARGS_TAM;
  char* delimitador = " \t\r\n\a";
  int i = 0;
  char** args = malloc(sizeof(char*) * bufsize);
  char* arg;

  arg = strtok(linha, delimitador);
  do {
    args[i] = arg;
    arg = strtok(NULL, delimitador);
    i++;

    if (i >= bufsize) {
      bufsize += ARGS_TAM;
      args = realloc(args, bufsize * sizeof(char*));
    }
  } while (arg != NULL);

  args[i] = NULL;
  return args;
}

//Funcao que proporciona caminhar com CD nos diretorios
void shell_cd(char** args) {
  if (args[1] == NULL) {
    fprintf(stderr, "myshell: esperado argumento para \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("myshell");
    }
  }
}

//retorna o tamanho de quantos argumentos possui o comando ex: #ls# #-a# = 2
int getLenght(char** vetor) {
  int count = 0;

  while (vetor[count] != NULL) {
    count++;
  }
  return count;
}

//retorna o comando do historico ex: !0 retorna o primeiro comando
char** executeHistory(char** args) {
  if (strcmp(args[0], "!!") == 0) {
    return readArgs(h->comands[i - 1]);
  } else {
    return readArgs(h->comands[args[0][1] - '0']);
  }
}

//Executa o comando, caso for cd, executa a funcao shell_cd.
void executarComando(char** args) {
  pid_t pid = fork();
  int status;
  int vetArgsSize = getLenght(args);

  if (args[0] == NULL || strcmp(args[0], "exit") == 0) {
    return;
  }
  if (strchr(args[0], '!') != NULL && pid == 0) {
    args = executeHistory(args);
  }

  if (strcmp(args[0], "cd") == 0) {
    shell_cd(args);
    return;
  }
  if (pid) {
    if (*args[vetArgsSize - 1] != '&') {
      wait(NULL);
    }
  } else {
    if (*args[vetArgsSize - 1] == '&') {
      args[vetArgsSize - 1] = NULL;
    }
    if (execvp(args[0], args) == -1) {
      perror("myshell");
    }
    exit(EXIT_FAILURE);
  }
}
int main() {
  int pid;
  char* keybord;
  char** args;
  h = initHistory(60);
  while (1) {
    printf("myshell@myshell>");
    keybord = readLine();
    if (strcmp(keybord, "!!") != 0) {
      printf("keyboard %s\n", keybord);
      h->comands[i] = malloc(strlen(keybord) + 1);
      strcpy(h->comands[i], keybord);
      i++;
    }
    args = readArgs(keybord);

    if (args[0] != NULL && strcmp(args[0], "exit") == 0) {
      break;
    }
    executarComando(args);
    free(keybord);
    free(args);
  }
}