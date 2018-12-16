/*
Autores: Guilherme Ianhez, Augusto Dias
Data: 06/11/2018
Projeto: https://github.com/AVFD/Escalonador-SO
 */
package projeto_soa;

import algoritmos.Prioridade;
import algoritmos.RoundRobin;
import algoritmos.Sjf;
import cpu.Escalonador;
import cpu.Processador;
import java.util.LinkedList;
import leituraArquivo.LerArquivo;
import processo.BCP;

public class Projeto_SOA {

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        //pega o nome do arquivo por argumento
        LerArquivo.file1 = args[0];
        // Instanciação dos 3 algoritmos e da lista de algoritmos para serem executados na plasse Processador.
        Escalonador sjf = new Sjf();
        Escalonador pri = new Prioridade();
        Escalonador roundRobin = new RoundRobin();
        LinkedList<Escalonador> listaAlg = new LinkedList<>();
        listaAlg.add(sjf);
        listaAlg.add(pri);
        listaAlg.add(roundRobin);
        Processador p = new Processador(listaAlg);

        //realiza escalonamento.
        p.processar();
        int i = 0;
        for (LinkedList<BCP> lk : p.getOrdemExecutados()) {
            System.out.println(p.getListaAlg().get(i).getClass().getName());
            for (BCP processo : lk) {
                processo.printProcesso();

            }
            System.out.println("\nTamanho MAX fila Aptos: " + p.getListaAlg().get(i).getTamanhoMaxFilaAptos());
            System.out.println("Tamanho MAX fila Bloqueados: " + p.getListaAlg().get(i).getTamanhoMaxFilaBlock());
            for (BCP p2 : p.getListaAlg().get(i).getListaFinalizados()) {
                System.out.println("P" + p2.getId());
                System.out.println("Tempo de espera Médio: " + p2.getTempoEsperaMedio());
                System.out.println("Tempo de espera Total: " + p2.getTempoEsperaTotal());
            }
            System.out.println("\n-------");
            i++;
        }

    }

}
