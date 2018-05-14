/*Falta fazer a parte de erros. A macro esta em um outro arquivo. Esta em um arquivo separado mas copiei algumas funcoes que voce 
usou.*/
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

void ifequ (list <tabSimItem> *tabSim, string nome){//Aqui tem o token e o token aux, sendo o token aux o  token anterior ao token
	ifstream arquivo;								//Para facilitar quando achar um EQU
    ofstream codprep;								//Quando for fazer a parte de erros tem que mudar um pouqinho	
    string token,linha,tokenaux;					
    int equflag,ifflag,nextlineflag,sectiontextflag,sectiondataflag,flagsimdefinido,contarg;
    int i=0, endereco=0, op=-1, simEndereco=-1,lala,linhacont,contlabel;
    tabSimItem SimAtual;

    arquivo.open("bin.asm");
    codprep.open("prebin.txt");
    linhacont = 1;
    sectiontextflag = 0;
    sectiondataflag = 0;
    equflag = 0;
    ifflag = 0;
    nextlineflag = 0;
    flagsimdefinido = 0;
    contarg=0;

    while (getline(arquivo,linha)) {
    	if (linha.length() == 0) {
    		linhacont++;
            continue;
        }
        stringstream linhaStream(linha);
        contarg = 0;
        contlabel = 0;
        while(linhaStream >> token){
        	//errotoken
        	if((token.back()) == ':'){ //Verifica se tem mais de uma label na linha
        		contlabel++;
        	}
			if((iequals(tokenaux,"SECTION") == 1) && iequals(token,"TEXT") == 1){
				sectiontextflag = 1; //Essa flag mostra que esta na SECTION TEXT
			}
			if((iequals(tokenaux,"SECTION") == 1) && iequals(token,"DATA") == 1){
				sectiontextflag = 0;
				sectiondataflag = 1; //ESSA flag mostra que esta na SECTION DATA
			}
        	if((iequals(token,"EQU") == 1)){//Verifica se o token e um EQU, se for EQU ele manda o token aux anterior pra tabela
        		equflag = 1;
        		if(sectiontextflag == 1 || sectiondataflag == 1){ //Verifica se o equ esta antes de todas as secoes
        			cout << "Erro semantico - EQU na secao errada. Linha: " << linhacont << endl; // Se nao estiver da erro
        		}
        			while(linhaStream >> token){//Pega os proximos tokens da linha do EQU
        				contarg++;//Conta quantos tokens tem no equ
        				//erro token
        				if((token.front()) == ';'){//Quando acha um ponto e virgula ele ignora o resto da linha 
        					contarg--;
        					while(linhaStream >> token)
        					continue;
        				}else{
        					if ((tokenaux.back()) == ':' && contarg == 1) {//Verifica se o tokenaux e uma label, se nao for da erro
            					tokenaux.pop_back(); //tira o :
           						simEndereco = tabSimSeek(*tabSim, tokenaux); //procura na tabela de simbolo
            					if (simEndereco > 0) {//verifica se o simbolo ja foi defindo
            						flagsimdefinido = 1;
                					cout << "Erro semantico - Simbolo ja definido: " << tokenaux << " Linha: " << linhacont << endl;
            					}else{
            						tokenaux.push_back('\0');
            						tokenaux.copy(SimAtual.label, tokenaux.length());
            					}
            					tokenaux.push_back(':');
       						}else if (contarg == 1){
       							cout << "Erro sintatico - Token anterior ao EQU nao e um label. Linha: " << linhacont << endl;
       						}
       						if(flagsimdefinido == 0 && contarg == 1){
       							SimAtual.endereco = atoi(token.c_str());
       							tabSim->push_back(SimAtual);
       						}else{
       							flagsimdefinido = 0;
       						}
       					}
       				}
       				if(contarg == 0){
       					cout << "Erro sintatico - EQU sem argumentos. Linha: " << linhacont << endl;
       				}else if (contarg > 1){
       					cout << "Erro sintatico - EQU com muitos argumentos. Linha: " << linhacont << endl;
       				}			
        	}
         	if((iequals(token,"IF") == 1)){
        		ifflag = 1;
        		if(sectiontextflag != 1){
        			cout << "Erro semantico - IF secao errada. Linha: " << linhacont << endl;
        		}
        		if((tokenaux.length()) != 0){
        			cout << "Erro sintatico - Tem coisa antes do IF. Linha: " << linhacont << endl;
        		}
        		while(linhaStream >> token){	
        			//erro token
        			contarg ++;
        			if((token.front()) == ';'){
        				contarg--;
        				while(linhaStream >> token)
        				continue;
        			}else if((lala = tabSimSeek(*tabSim, token)) >= 0 && contarg == 1){
        				if(lala != 1){
        					nextlineflag = 1;
        				}
					}else if(lala == -1 && contarg == 1){
       					cout << "Erro semantico - Simbolo nao definido. Linha: " << linhacont << endl;
					}
        		}
        		if (contarg == 0){
        			cout << "Erro sintatico - IF sem argumentos. Linha: " << linhacont << endl;
        		}else if (contarg > 1){
        			cout << "Erro sintatico - IF com muitos argumentos. Linha: " << linhacont << endl;
        		}
        	}
        	tokenaux = token;
        }
        tokenaux.clear();   
        if(equflag == 1 || ifflag == 1){
        	equflag = 0;
        	ifflag = 0;
        }
        else if(nextlineflag == 1){
        		nextlineflag = 0;
        	}
        	else {
        		if(linha.find(';')<1000){ //Quando acha um ';', diminiu o tamanho da string ate antes do ';'
        			linha.resize(linha.find(';')-1);
        		}
        		codprep << linha << "\n";
        	}  
        //scanner(token);
        if(contlabel > 1){
        	cout << "Erro sintatico - Mais de 1 label na linha. Linha: " << linhacont << endl;
        }
        linhacont++;
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