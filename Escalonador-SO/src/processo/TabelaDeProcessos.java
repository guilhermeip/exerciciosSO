package processo;

import java.util.LinkedList;
import leituraArquivo.LerArquivo;

public class TabelaDeProcessos {

    public static TabelaDeProcessos tp = null;
    LinkedList<BCP> tabelaDeProcesso;

    public TabelaDeProcessos() {
        this.tabelaDeProcesso = new LinkedList<>();
    }

    public static TabelaDeProcessos getInstance() {
        tp = new LerArquivo().lerArquivo();
        return tp;
    }

    public LinkedList<BCP> getTabelaDeProcesso() {
        return tabelaDeProcesso;
    }

    public void setTabelaDeProcesso(LinkedList<BCP> tabelaDeProcesso) {
        this.tabelaDeProcesso = tabelaDeProcesso;
    }

}
