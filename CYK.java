import java.io.FileNotFoundException;
import java.io.FileReader;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Scanner;

public class CYK {
    static HashMap<String, ArrayList<String>> variables = new HashMap<String, ArrayList<String>>();
    static HashMap<String, ArrayList<String>> terminals = new HashMap<String, ArrayList<String>>();
    static ArrayList<String> non_term = new ArrayList<String>();
    static String _start;

    public static void main(String [] args) throws FileNotFoundException {
        FileReader fr = new FileReader("input.txt");
        Scanner sc = new Scanner(fr);
        boolean flag=false;

        while(sc.hasNextLine()) {
            String line[] = sc.nextLine().split("[-|]");
            if(!flag) {

                _start = line[0];
                flag = true;
            }
            non_term.add(line[0]);
            variables.put(line[0], new ArrayList<String>());
            terminals.put(line[0], new ArrayList<String>());
                for(int i=1; i<line.length; i++) {
                    if (line[i].charAt(0) >= 'a' && line[i].charAt(0) <= 'z') {
                        terminals.get(line[0]).add(line[i]);
                    } else {
                        variables.get(line[0]).add(line[i]);
                    }
                }
            }

            int p = variables.size();
            String word = "baaba";
            int n = word.length();

            boolean P[][][] = new boolean[n+1][n+1][p+1];

            for(int i=1; i<=n; i++) {
                for(int j=1; j<=p; j++) {

                    if(terminals.get(non_term.get(j-1)).contains(word.charAt(i-1)+"")) {
                        P[1][i][j] = true;
                    }
                }
            }
            for(int i=2; i<=n; i++) {
                for(int j=1; j<=n-i+1; j++) {

                    for(int k=1; k<=i-1; k++) {
                        for(String v: non_term) {

                            int indexA = findIndex(v);
                            for(String prod: variables.get(v)) {
                                int indexB = findIndex(prod.charAt(0)+"");
                                int indexC = findIndex(prod.charAt(1)+"");
                                if(P[k][j][indexB] && P[i-k][j+k][indexC]) {

                                    P[i][j][indexA] = true;
                                }
                            }
                        }
                    }
                }
            }
            for(int i=1; i<=n; i++) {
                for(int j=1; j<=n; j++) {
                    for(int k=1; k<=p; k++) {
                        if(P[i][j][k]) {
                            System.out.print(non_term.get(k-1)+",");
                        }
                    }
                    System.out.print(" ");
                }
                System.out.println("");
            }
            if(P[n][1][findIndex(_start)]) {
                System.out.println(word + " is member of grammar");

            } else {
                System.out.println(word + " is NOT a member of grammar");
            }
        }
        static int findIndex(String s) {
            for(int i=1; i<= non_term.size(); i++) {
                if(s.equals(non_term.get(i-1))) {
                    return i;
            }
        }
        return -1;
    }

}
