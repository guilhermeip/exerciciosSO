package cpu;

import java.util.LinkedList;
import processo.BCP;
import processo.TabelaDeProcessos;

public abstract class Escalonador {

    LinkedList<BCP> listaBloqueado;
    LinkedList<BCP> listaAptos;
    LinkedList<BCP> listaProcessos;
    LinkedList<BCP> listaFinalizados;
    int tamanhoMaxFilaAptos;
    int tamanhoMaxFilaBlock;

    public Escalonador() {
        this.listaAptos = new LinkedList<>();
        this.listaBloqueado = new LinkedList<>();
        this.listaFinalizados = new LinkedList<>();
        this.listaProcessos = TabelaDeProcessos.getInstance().getTabelaDeProcesso();
    }

    abstract public BCP escalonar();

    public LinkedList<BCP> getListaBloqueado() {
        return listaBloqueado;
    }

    public LinkedList<BCP> getListaFinalizados() {
        return listaFinalizados;
    }

    public void setListaFinalizados(LinkedList<BCP> listaFinalizados) {
        this.listaFinalizados = listaFinalizados;
    }

    public void setListaBloqueado(LinkedList<BCP> listaBloqueado) {
        this.listaBloqueado = listaBloqueado;
    }

    // Retornamos o Indice de um processo em alguma lista, baseado no ID do processo.
    public int returnIndexProcessoNaLista(int id, LinkedList<BCP> lista) {
        for (BCP p : lista) {
            if (p.getId() == id) {
                return lista.indexOf(p);
            }
        }
        return -1;
    }
    
    // Monta a lista de processo aptos.
    public void retornarProcessosAptos(int ciclo) {

        for (BCP p : this.listaProcessos) {
            if (!p.isBlocked && p.getTempoChegada() <= ciclo && !(processoJaApto(p.getId()))) {
                this.listaAptos.add(p);
            }
        }

    }

    // Retorna se um processo está ou não na lista de processos aptos.
    public boolean processoJaApto(int indiceProcesso) {
        boolean tem = false;
        for (BCP p : this.listaAptos) {
            if (p.getId() == indiceProcesso) {
                tem = true;
            }
        }
        return tem;
    }

    public LinkedList<BCP> getListaAptos() {
        return listaAptos;
    }

    public void setListaAptos(LinkedList<BCP> listaAptos) {
        this.listaAptos = listaAptos;
    }

    public LinkedList<BCP> getListaProcessos() {
        return listaProcessos;
    }

    public void setListaProcessos(LinkedList<BCP> listaProcessos) {
        this.listaProcessos = listaProcessos;
    }

    // Remove um processo de uma lista de processos baseado no ID.
    public void removeById(int id, LinkedList<BCP> listaParaRemover) {
        for (int i = 0; i < listaParaRemover.size(); i++) {
            if (id == listaParaRemover.get(i).getId()) {
                listaParaRemover.remove(i);
            }
        }
    }
    
    public void tamanhoMaxAptos(){
        if (this.tamanhoMaxFilaAptos < this.listaAptos.size()){
            this.tamanhoMaxFilaAptos = this.listaAptos.size();
        }
    }
    
    public void tamanhoMaxBloq(){
        if (this.tamanhoMaxFilaBlock < this.listaBloqueado.size()){
            this.tamanhoMaxFilaBlock = this.listaBloqueado.size();
        }
    }

    public int getTamanhoMaxFilaAptos() {
        return tamanhoMaxFilaAptos;
    }

    public void setTamanhoMaxFilaAptos(int tamanhoMaxFilaAptos) {
        this.tamanhoMaxFilaAptos = tamanhoMaxFilaAptos;
    }

    public int getTamanhoMaxFilaBlock() {
        return tamanhoMaxFilaBlock;
    }

    public void setTamanhoMaxFilaBlock(int tamanhoMaxFilaBlock) {
        this.tamanhoMaxFilaBlock = tamanhoMaxFilaBlock;
    }
    
}
