package algoritmos;

import cpu.Processador;
import processo.BCP;

public class RoundRobin extends cpu.Escalonador {

    public RoundRobin() {
        super();
    }

    @Override
    public BCP escalonar() {
        BCP bcp = new BCP();
        /*Esta chamada de função faz com que a lista de processos aptos seja montada, 
        baseado na relação de ciclo do processador e o tempo de chegada do processo.*/
        retornarProcessosAptos(Processador.ciclo);

        if (this.getListaAptos().isEmpty()) {
            return null;
        }
        
        // Selecionar o processo a ser escalonado, baseado no menor tempo de execução.
        bcp = this.getListaAptos().remove(0);
        this.getListaAptos().add(bcp);
        return bcp;
    }

}
