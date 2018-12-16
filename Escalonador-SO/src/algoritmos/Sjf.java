package algoritmos;

import cpu.Processador;
import processo.BCP;

public class Sjf extends cpu.Escalonador {

    public Sjf() {
        super();
    }

    // Método de escalonamento do algoritmo SJF.
    @Override
    public BCP escalonar() {
        /*Esta chamada de função faz com que a lista de processos aptos seja montada, 
        baseado na relação de ciclo do processador e o tempo de chegada do processo.*/
        retornarProcessosAptos(Processador.ciclo);

        if (this.getListaAptos().isEmpty()) {
            return null;
        }

        // Selecionar o processo a ser escalonado, baseado no menor tempo de execução.
        BCP bcp = this.getListaAptos().get(0);
        for (BCP p : this.getListaAptos()) {
            if (bcp.getTempoTotal() > p.getTempoTotal()) {
                bcp = p;
            }
        }
        return bcp;
    }
}
