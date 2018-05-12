/*Falta os erros, comentario, macro. Esta em um arquivo separado mas copiei algumas funcoes que voce usou.*/
#include <iostream>
#include <fstream>                           //Para lidar com o arquivo. Nunca usei isso, então não sei comofas
#include <string.h>
#include <list>
#include <ctype.h>                          // Usando na função isalpha
#include <sstream>                          // Para retirar as palavras da linha
#include <algorithm>                        // Para procurar na tabela de simbolos
#include <string>

using namespace std;

typedef struct tabSimItem_s {
    char label[20];
    int endereco;
} tabSimItem;

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
void printaTabSimTemp (list <tabSimItem> tabSim) {
    string texto;
    list <tabSimItem> :: iterator it;

    it = tabSim.begin();
    while (it != tabSim.end()) {
        texto = it->label;
        cout << "Label: " << texto << "\tEndereco: " << it->endereco << endl;
        it++;
    }
}

int tabSimSeek (list <tabSimItem> tabSim, string token) {
    list <tabSimItem> :: iterator it;

    it = tabSim.begin();
    while (1) {
        if (it == tabSim.end()) {
            return -1;
        }
        else if (iequals(token.c_str(),it->label) != 1) {
            it++;
        }
        else {
            return it->endereco;
        }
    }
}

void ifequ (list <tabSimItem> *tabSim, string nome){
	ifstream arquivo;
    ofstream codprep;
    string token,linha,tokenaux;
    int equflag,ifflag,nextlineflag,i=0, endereco=0, op=-1, simEndereco=-1,lala;
    tabSimItem SimAtual;

    arquivo.open("bin.asm");
    codprep.open("bin.txt");
    while (getline(arquivo,linha)) {
        stringstream linhaStream(linha);
        while(linhaStream >> token){
        	//errotoken
        	if((iequals(token,"EQU") == 1)){//Verifica se o token e um EQU, se for EQU ele manda o label anterior pra tabela
        		equflag = 1;
        		if ((tokenaux.back()) == ':') {
            		tokenaux.pop_back();
           			simEndereco = tabSimSeek(*tabSim, tokenaux);
            		if (simEndereco > 0) {
                		cout << "Erro, simbolo ja definido: |" << tokenaux << endl;
            		}
            		tokenaux.push_back('\0');
            		tokenaux.copy(SimAtual.label, tokenaux.length());
            		
       			}else{
       				cout << "Erro sintatico";
       			}
        	}
        	if((iequals(tokenaux,"EQU") == 1)){
        		SimAtual.endereco = atoi (token.c_str());
        		tabSim->push_back(SimAtual);
        		
        	}
        	if((iequals(tokenaux,"IF") == 1)){
        		ifflag = 1;
        		if((lala = tabSimSeek(*tabSim, token)) >= 0){
        			if(lala != 1){
        				nextlineflag = 1;
        			}
        		}
        	}
        	tokenaux = token;
        }   
        if(equflag == 1 || ifflag == 1){
        	equflag = 0;
        	ifflag = 0;
        }
        else if(nextlineflag == 1){
        		nextlineflag = 0;
        	}
        	else {
        		if(linha.find(';')<1000){
        			linha.resize(linha.find(';')-1);
        		}
        		codprep << linha << "\n";
        	}  
        //scanner(token);
    }
}

int main () {
    list <tabSimItem> tabSim;
    string nome="teste.asm";

    //inputTemp (&nome);
    ifequ(&tabSim,nome);
    //primeiraPassagem(&tabSim,nome);
    printaTabSimTemp(tabSim);
    //segundaPassagem(tabSim,nome);
    return 0;
}