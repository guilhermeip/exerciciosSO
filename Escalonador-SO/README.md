# Simulador do escalonador de processos
Simula três algoritmos (Short Job First - SJF, Priority, RoundRobin - RR) para os mesmos processos pré definidos em um arquivo txt. 
## Pré-requisitos

* Necessário java 7 ou posteriores.
* Passar por parâmetro (argumento) o nome do arquivo que contém as informações dos processos.
  * O arquivo deve seguir o seguinte padrão:
    * Cada processo será uma linha e atributos são separados por espaço.
    * Todos os atributos são números
    * 1º atributo: ID do processo
    * 2º atributo: Tempo total que o processo necessita para executar.
    * 3° atributo: Prioridade do processo
    * 4° atributo: Tempo de chegada a CPU
      * 5º ou mais atributos (opicional): será o tempo (ciclo do processador) em que o processo fará um procedimento de IO.
        * Tempo de IO = 1, significa que um processo qualquer que chegue no tempo 5 na CPU executará uma vez e fará IO
        * Tempo de IO = 2, significa que um processo qualquer que chegue no tempo 15 na CPU executará 2 vezes e depois realizará IO
    * ```
      Exemplo:
      1 5 2 1          //id:1 Tempo_Total:5 Prioridade:2 chegada_CPU:1
      2 2 1 5 1        //id:2 Tempo_Total:2 Prioridade:1 chegada_CPU:5 Lista_IO: 1 
      0 10 4 11 3 5 7  //id:0 Tempo_Total:10 Prioridade:4 chegada_CPU:11 Lista_IO: 3 5 7 
      3 3 8 3
      4 6 1 4 2 3 5
      ```
    

# Valores Padrões
* Quantum do processador = 1
* Tempo em que um processo realizará IO (bloqueado): 2

### Autores
Trabalho para disciplina de Sistemas Operacionais desenvolvido por:

Augusto Vicente Fernandes Dias

Guilherme Ianhez Pereira dos Santos
