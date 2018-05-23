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
    string macroarg[4];
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
        cout << "Label: " << texto << "\tArgumentos: " << it->nargumentos << "\t LinhaMDT: " <<  it->linhamdt << "\t LinhaMDTFIM: " << it->linhamdtfim << "\t MacroArg: " << it->macroarg[0] << "\t MacroArg: " << it->macroarg[1] << "\t MacroArg: " << it->macroarg[2] << "\t MacroArg: " << it->macroarg[3] << endl;
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

int tabSimSeek2 (list <MacroNameTable> MNT, string token,int *nargumentos, int *linhamdt, int *linhamdtfim, string *arg1, string *arg2, string *arg3, string *arg4) {
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
            *nargumentos = it->nargumentos;
            *linhamdt = it->linhamdt;
            *linhamdtfim = it->linhamdtfim;
            *arg1 = it->macroarg[0];
            *arg2 = it->macroarg[1];
            *arg3 = it->macroarg[2];
            *arg4 = it->macroarg[3];
            return it->linhamdtfim;
        }
    }
}

int tabSimSeek3 (list <MacroNameTable> MNT, string token) {
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
            return it->nargumentos;
        }
    }
}

void macro (list <MacroNameTable> *MNT, string nome){ //Um tanto desses inteiros eu copiei da sua parte e tenho medo de apagar
    ifstream arquivo;
    ofstream codprep;
    string token,linha,tokenaux,mdt[100],argmacro,argumentodeclarado[10],argumentochamado[10],mdtaux;
    int equflag,ifflag,nextlineflag,i=0, endereco=0, op=-1, simEndereco=-1,lala,macroflag=0,argumeto;
    int mdtcont=0,contarg=0,mdtsearch = 0, fim,z,macroflag2,endmacroflag,tirarlinha,mdtfim,trocaargumentos,nargumentos;
    MacroNameTable SimAtual;

    arquivo.open("prebin.txt");
    codprep.open("macrobin.txt");
    while (getline(arquivo,linha)) {
        contarg = 0;
        macroflag2 = 0;
        endmacroflag = 0;
        tirarlinha = 0;
        stringstream linhaStream(linha);
        while(linhaStream >> token){
            //errotoken
            if((tabSimSeek2(*MNT,token,&nargumentos,&mdtsearch,&mdtfim,&argumentodeclarado[0],&argumentodeclarado[1],&argumentodeclarado[2],&argumentodeclarado[3])) > -1){
                tirarlinha = 1;
                while(linhaStream >> token){
                    contarg++;
                    if (contarg == 1){
                        z = 0;
                        replace( token.begin(), token.end(), ',', ' ' );
                        stringstream tokenStream(token);
                        while(tokenStream >> argmacro){
                            argumentochamado[z] = argmacro;
                            z++;
                        }
                        if(z == nargumentos){
                            trocaargumentos = 1;
                        }else{
                        cout << "Erro, numero de argumentos invalidos" << endl;
                        }
                    }else{
                        cout << "ERRO" << endl;
                    }
                }
                for(mdtsearch;mdtsearch < mdtfim;mdtsearch++){
                    mdtaux = mdt[mdtsearch];
                    if(trocaargumentos == 1){
                        if(mdtaux.find(argumentodeclarado[0]) < 1000){
                                cout << argumentochamado[0] << endl;
                                mdtaux.replace(mdtaux.find(argumentodeclarado[0]),max(argumentochamado[0].length(),argumentodeclarado[0].length()),argumentochamado[0]);
                        }
                        if(mdtaux.find(argumentodeclarado[1]) < 1000){
                                mdtaux.replace(mdtaux.find(argumentodeclarado[1]),max(argumentochamado[1].length(),argumentodeclarado[1].length()),argumentochamado[1]);
                        }
                        if(mdtaux.find(argumentodeclarado[2]) < 1000){
                                mdtaux.replace(mdtaux.find(argumentodeclarado[2]),max(argumentochamado[2].length(),argumentodeclarado[2].length()),argumentochamado[2]);
                        }
                        if(mdtaux.find(argumentodeclarado[3]) < 1000){
                                mdtaux.replace(mdtaux.find(argumentodeclarado[3]),max(argumentochamado[3].length(),argumentodeclarado[3].length()),argumentochamado[3]);
                        }
                        if(mdtaux.find(argumentodeclarado[0]) < 1000){
                                mdtaux.replace(mdtaux.find(argumentodeclarado[0]),max(argumentochamado[0].length(),argumentodeclarado[0].length()),argumentochamado[0]);
                        }
                        if(mdtaux.find(argumentodeclarado[1]) < 1000){
                                mdtaux.replace(mdtaux.find(argumentodeclarado[1]),max(argumentochamado[1].length(),argumentodeclarado[1].length()),argumentochamado[1]);
                        }
                        if(mdtaux.find(argumentodeclarado[2]) < 1000){
                                mdtaux.replace(mdtaux.find(argumentodeclarado[2]),max(argumentochamado[2].length(),argumentodeclarado[2].length()),argumentochamado[2]);
                        }
                        if(mdtaux.find(argumentodeclarado[3]) < 1000){
                                mdtaux.replace(mdtaux.find(argumentodeclarado[3]),max(argumentochamado[3].length(),argumentodeclarado[3].length()),argumentochamado[3]);
                        }
                    }

                    codprep << mdtaux << endl;
                }
            }
            if((iequals(token,"MACRO") == 1)){//Verifica se o token e um EQU, se for EQU ele manda o label anterior pra tabela
                macroflag = 1;//Seta uma flag de macro = 1
                macroflag2 = 1;
                if ((tokenaux.back()) == ':') {//Verifica se o token anterior a macro e um label
                    tokenaux.pop_back();
                    simEndereco = tabSimSeek2(*MNT, tokenaux,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
                    if (simEndereco > 0) {
                        cout << "Erro, simbolo ja definido: |" << tokenaux << endl;
                    }
                    tokenaux.push_back('\0');
                    tokenaux.copy(SimAtual.label, tokenaux.length());
                    SimAtual.linhamdt = mdtcont;
                    SimAtual.nargumentos = 0;
                }
                else{
                        cout << "Erro, nao eh label";
                }
                while(linhaStream >> token){
                    contarg++;
                    if (contarg == 1){
                        replace( token.begin(), token.end(), ',', ' ' );
                        stringstream tokenStream(token);
                        z = 0;
                        while(tokenStream >> argmacro){
                            if (argmacro.front() != '&'){
                                cout << "Argumento na forma invalida" << endl;
                            }else{
                                SimAtual.macroarg[z] = argmacro;
                                z++;
                            }
                        }
                        if(z>4){
                            cout << "Macro com muitos argumentos" << endl;
                        }else{
                            SimAtual.nargumentos = z;
                            z = 0;
                        }
                    }
                }
                if(contarg > 1){
                    cout << "Macro com muitos argumentos" << endl;
                }else{
                }
            }
            if((iequals(token,"ENDMACRO") == 1)){
                macroflag = 0; //Seta a flag de macro = 0
                endmacroflag = 1;
                SimAtual.linhamdtfim = mdtcont;
                MNT->push_back(SimAtual);
            }
            tokenaux = token;
        }
        if (macroflag == 1 && macroflag2 == 0){ //Quando acha a label MACRO, copia as proximas linhas ate o ENDMACRO pro MDT(arquivo texto)
            mdt[mdtcont] = linha;//E conta a linha pra colocar na MNT
            mdtcont++;
        }
        if(macroflag == 0 && endmacroflag == 0 && tirarlinha == 0){
            codprep << linha << endl;
        }
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
