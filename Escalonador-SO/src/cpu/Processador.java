package cpu;

import java.util.LinkedList;
import processo.BCP;

public class Processador {

    public static int cicloIO = 2;

    private LinkedList<Escalonador> listaAlg;
    private LinkedList<LinkedList<BCP>> ordemExecutados;
    public static int ciclo = 0;
    private int quantum = 1;

    public Processador(LinkedList<Escalonador> listaAlg) {
        this.listaAlg = listaAlg;
        this.ordemExecutados = new LinkedList<>();
    }

    public void processar() {
        int size = listaAlg.size();
        //For para rodar em toda a lista de algoritmos que foram inseridos.
        for (int i = 0; i < size; i++) {
            Escalonador atual = listaAlg.get(i);
            LinkedList<BCP> listaAtualExecutados = new LinkedList<>();
            //Executar o laço DO_WHILE até que os processos tenham sido encerrados.
            do {
                //Aqui o escalonamento de cada algoritmo é chamado, e então retorna o processo escalonado.
                BCP p = atual.escalonar();
                int tempoExecutado;
                //Quando se retorna um Processo para ser escalonado.
                if (p != null) {
                    listaAtualExecutados.add(p);
                    p.jaExecutou = true;
                    if ((p.getTempoTotal() - quantum) < 0) {
                        tempoExecutado = p.getTempoTotal();
                        ciclo += p.getTempoTotal();
                    } else {
                        tempoExecutado = quantum;
                        ciclo += quantum;
                    }
                    p.setTempoTotal(p.getTempoTotal() - quantum);

                    if (p.getTempoTotal() <= 0) {
                        atual.getListaFinalizados().add(p);
                        atual.getListaProcessos().remove(p);
                        atual.getListaAptos().remove(p);
                    }

                } else /* Caso nenhum processo tenha sido escalonado */ {
                    tempoExecutado = 1;
                    ciclo++;
                }
                incrementListaEspera(atual, p, tempoExecutado);

                // Realizar decremento na lista de processo bloqueados.
                decrementarBloqueados(atual, tempoExecutado);
                // Caso o processo tenha que ser bloqueado.
                if (p != null && p.needBlock(ciclo)) {
                    int indexP = atual.returnIndexProcessoNaLista(p.getId(), atual.getListaAptos());
                    atual.listaBloqueado.add(atual.listaAptos.remove(indexP));
                }
                atual.tamanhoMaxAptos();
                atual.tamanhoMaxBloq();

            } while (!atual.getListaProcessos().isEmpty());
            ordemExecutados.add(listaAtualExecutados);
            Processador.ciclo = 0;
        }
    }

    private void incrementListaEspera(Escalonador es, BCP bcp, int tempoExecutado) {
        for (BCP p : es.listaProcessos) {
            if (bcp != null && p.getId() != bcp.getId() && p.jaExecutou  ) {
                p.setTempoEsperaTotal(p.getTempoEsperaTotal() + tempoExecutado);
            }
        }
    }

    // Esta função compara se os processos que estão bloqueados podem voltar para a lista de aptos.
    private void decrementarBloqueados(Escalonador es, int tempoExecutado) {
        for (BCP p : es.getListaBloqueado()) {
            p.tempoIO -= tempoExecutado;
            if (p.tempoIO <= 0) {
                p.tempoIO = Processador.cicloIO;
                p.isBlocked = false;
                int indexP = es.returnIndexProcessoNaLista(p.getId(), es.listaBloqueado);
                es.listaAptos.add(es.listaBloqueado.remove(indexP));
            }
        }
    }

    public LinkedList<Escalonador> getListaAlg() {
        return listaAlg;
    }

    public void setListaAlg(LinkedList<Escalonador> listaAlg) {
        this.listaAlg = listaAlg;
    }

    public LinkedList<LinkedList<BCP>> getOrdemExecutados() {
        return ordemExecutados;
    }

    public void setOrdemExecutados(LinkedList<LinkedList<BCP>> ordemExecutados) {
        this.ordemExecutados = ordemExecutados;
    }

}
