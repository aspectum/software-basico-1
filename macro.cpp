/*Ate agora ele acha a label macro, coloca na tabela MNT. Esta copiando o codigo da macro em um arquivo TXT(MDT) e contando as linhas
para colocar na MNT(Nao pensei ainda em outra forma melhor de MDT). Ele coloca a linha referente a MDT na MNT. Ainda nao contei
argumentos nem expandi a macro quando chama.*/
#include <iostream>
#include <fstream>                           //Para lidar com o arquivo. Nunca usei isso, então não sei comofas
#include <string.h>
#include <list>
#include <ctype.h>                          // Usando na função isalpha
#include <sstream>                          // Para retirar as palavras da linha
#include <algorithm>                        // Para procurar na tabela de simbolos
#include <string>

using namespace std;

typedef struct MacroNameTable {
    char label[20];
    int nargumentos;
    int linhamdt;
    int linhamdtfim;
} MacroNameTable;


bool icompare_pred(unsigned char a, unsigned char b)
{
    return std::tolower(a) == std::tolower(b);
}

bool iequals(std::string const& a, std::string const& b)
{
    if (a.length()==b.length()) {
        return std::equal(b.begin(), b.end(),
                           a.begin(), icompare_pred);
    }
    else {
        return false;
    }
}
void printaTabSimTemp (list <MacroNameTable> MNT) {
    string texto;
    list <MacroNameTable> :: iterator it;

    it = MNT.begin();
    while (it != MNT.end()) {
        texto = it->label;
        cout << "Label: " << texto << "\tArgumentos: " << it->nargumentos << "\t LinhaMDT: " <<  it->linhamdt << "\t LinhaMDTFIM: " << it->linhamdtfim << endl;
        it++;
    }
}

int tabSimSeek (list <MacroNameTable> MNT, string token) {
    list <MacroNameTable> :: iterator it;

    it = MNT.begin();
    while (1) {
        if (it == MNT.end()) {
            return -1;
        }
        else if (iequals(token.c_str(),it->label) != 1) {
            it++;
        }
        else {
            return it->linhamdt;
        }
    }
}

void macro (list <MacroNameTable> *MNT, string nome){ //Um tanto desses inteiros eu copiei da sua parte e tenho medo de apagar
    ifstream arquivo;
    ofstream codprep;
    string token,linha,tokenaux,mdt[100];
    int equflag,ifflag,nextlineflag,i=0, endereco=0, op=-1, simEndereco=-1,lala,macroflag=0,argumeto;
    int mdtcont=0,contarg=0,mdtsearch = 0;
    MacroNameTable SimAtual;

    arquivo.open("prebin.txt");
    codprep.open("macrobin.txt");
    while (getline(arquivo,linha)) {
        contarg = 0;
        stringstream linhaStream(linha);
        if (macroflag == 1){ //Quando acha a label MACRO, copia as proximas linhas ate o ENDMACRO pro MDT(arquivo texto)
            mdt[mdtcont] = linha;//E conta a linha pra colocar na MNT
            mdtcont++;
        }
        while(linhaStream >> token){
            //errotoken
            if((simEndereco = tabSimSeek(*MNT,token)) > -1){
                for(mdtsearch = SimAtual.linhamdt;mdtsearch < SimAtual.linhamdtfim;mdtsearch++){
                    cout << mdt[mdtsearch] << "\n";
                }
            }
            if((iequals(token,"MACRO") == 1)){//Verifica se o token e um EQU, se for EQU ele manda o label anterior pra tabela
                macroflag = 1;//Seta uma flag de macro = 1
                if ((tokenaux.back()) == ':') {//Verifica se o token anterior a macro e um label
                    tokenaux.pop_back();
                    simEndereco = tabSimSeek(*MNT, tokenaux);
                    if (simEndereco > 0) {
                        cout << "Erro, simbolo ja definido: |" << tokenaux << endl;
                    }
                    tokenaux.push_back('\0');
                    tokenaux.copy(SimAtual.label, tokenaux.length());
                    SimAtual.linhamdt = mdtcont;
                }else{cout << "Erro, nao eh label";}
                while(linhaStream >> token){
                    contarg++;
                    if (contarg == 1){
                        //while(token.replace(token.find(','),1," "))
                    }
                }
                if(contarg > 1){
                    cout << "Macro com muitos argumentos" << endl;
                }else{
                }
            }
            if((iequals(token,"ENDMACRO") == 1)){
                macroflag = 0; //Seta a flag de macro = 0
                SimAtual.linhamdtfim = mdtcont;
                MNT->push_back(SimAtual);
            }
            tokenaux = token;
        }
    }
    cout << endl;
    cout << endl;
    cout << endl;
    cout << endl;
    for(i=0;i<10;i++){
        cout << mdt[i] << endl;
    }
}

int main () {
    list <MacroNameTable> MNT;
    string nome="teste.asm";

    //inputTemp (&nome);
    //ifequ(&tabSim,nome);
    macro(&MNT,nome);
    //primeiraPassagem(&tabSim,nome);
    printaTabSimTemp(MNT);
    //segundaPassagem(tabSim,nome);

    return 0;

}
