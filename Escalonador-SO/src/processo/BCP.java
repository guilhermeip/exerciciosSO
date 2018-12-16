package processo;

import cpu.Processador;
import java.util.LinkedList;
import leituraArquivo.LerArquivo;

public class BCP {
    private int id;
    private int prioridade;
    private String estado;
    private int tempoChegada;
    public boolean jaExecutou = false;
    private int tempoTotal;
    private int tempoEsperaMedio = 0;
    private int tempoEsperaTotal = 0;
    public int tempoIO = Processador.cicloIO;
    public boolean isBlocked = false;
    private LinkedList<Integer> listaIO;
    public BCP(){
        this.listaIO = new LinkedList<>();
    }

    public int getId() {
        return id;
    }

    public int getTempoEsperaMedio() {
        return tempoEsperaMedio;
    }

    public void setTempoEsperaMedio(int tempoEsperaMedio) {
        this.tempoEsperaMedio = tempoEsperaMedio;
    }

    public int getTempoEsperaTotal() {
        return tempoEsperaTotal;
    }

    public void setTempoEsperaTotal(int tempoEsperaTotal) {
        this.tempoEsperaTotal = tempoEsperaTotal;
        this.tempoEsperaMedio = tempoEsperaTotal / LerArquivo.quantProcessos;
    }

    public void setId(int id) {
        this.id = id;
    }

    public int getPrioridade() {
        return prioridade;
    }

    public void setPrioridade(int prioridade) {
        this.prioridade = prioridade;
    }

    public String getEstado() {
        return estado;
    }

    public void setEstado(String estado) {
        this.estado = estado;
    }
    
    public boolean needBlock(int ciclo){
        for(int i = 0; i < listaIO.size(); i++){
            if(tempoTotal > 0 && listaIO.get(i) <= ciclo){
                listaIO.remove(i);
                isBlocked = true;
                return true;
            }
        }
        return false;
    }
    public int getTempoChegada() {
        return tempoChegada;
    }

    public void setTempoChegada(int tempo) {
        this.tempoChegada = tempo;
    }
    public int getTempoTotal() {
        return tempoTotal;
    }

    public void setTempoTotal(int tempo) {
        this.tempoTotal = tempo;
    }

    public LinkedList<Integer> getListaIO() {
        return listaIO;
    }

    public void setListaIO(LinkedList<Integer> listaIO) {
        this.listaIO = listaIO;
    }
    
    public void printProcesso(){
        System.out.print("P" + this.getId()+" ");
        //System.out.println("Tamanho: " + this.getTempoTotal());
        //System.out.println("Prioridade: " + this.getPrioridade());
        //System.out.println("TempoChegada: " + this.getTempoChegada());
    }
    
    public void copiaProcesso(BCP processo){
        this.id = processo.id;
        this.estado = processo.estado;
        this.listaIO = processo.listaIO;
        this.prioridade = processo.prioridade;
        this.tempoChegada = processo.tempoChegada;
        this.tempoTotal = processo.tempoTotal;
    }
    
}
