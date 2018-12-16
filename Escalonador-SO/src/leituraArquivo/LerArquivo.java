package leituraArquivo;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;
import processo.BCP;
import processo.TabelaDeProcessos;

public class LerArquivo {

    public static String file1;
    public static int quantProcessos = 0;

    public TabelaDeProcessos lerArquivo() {
        TabelaDeProcessos tabelaDeProcessos = new TabelaDeProcessos();
        try {
            FileReader arq = new FileReader(file1);
            BufferedReader lerArq = new BufferedReader(arq);
            String line = "";
            String parts[];

            while ((line = lerArq.readLine()) != null) {
                BCP novoProcesso = new BCP();
                quantProcessos++;
                System.out.println("--> " + line);
                parts = line.split(" ");
                novoProcesso.setId(Integer.valueOf(parts[0]));
                novoProcesso.setTempoTotal(Integer.valueOf(parts[1]));
                novoProcesso.setPrioridade(Integer.valueOf(parts[2]));
                novoProcesso.setTempoChegada(Integer.valueOf(parts[3]));
                // Este for executa caso o processo tenha uma lista de IO.
                for (int i = 4; i < parts.length; i++) {
                    novoProcesso.getListaIO().add(Integer.valueOf(parts[i]));
                }
                tabelaDeProcessos.getTabelaDeProcesso().add(novoProcesso);
            }
            System.out.println("---------");

        } catch (FileNotFoundException ex) {
            System.out.println("Na classe LerAlgoritmos, não foi encontrado o arquivo com esse nome: " + file1);
            Logger.getLogger(LerArquivo.class.getName()).log(Level.SEVERE, null, ex);
        } catch (IOException ex) {
            Logger.getLogger(LerArquivo.class.getName()).log(Level.SEVERE, null, ex);
        }

        return tabelaDeProcessos;
    }
}
