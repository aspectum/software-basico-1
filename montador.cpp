/*
--------------------------------------VERSAO ATUAL--------------------------------------
* Juntando os arquivos
- TabsimItem do pre.cpp ta sem flagtipo, dá pau? 
----------------------------------------------------------------------------------------
*/
#include <iostream>
#include <fstream>                           //Para lidar com o arquivo. Nunca usei isso, então nao sei comofas
#include <string.h>
#include <list>
#include <ctype.h>                          // Usando na função isalpha
#include <sstream>                          // Para retirar as palavras da linha
#include <algorithm>                        // Para procurar na tabela de simbolos

using namespace std;

/*
O campo tipo é uma flag
'X' - valor padrao
'0' - const 0 (nao pode dividir por nem modificar)
'C' - const (nao pode modificar)
'D' - label na seção data (nao pode dar jump)
*/
typedef struct MacroNameTable {
    char label[20];
    int nargumentos;
    int linhamdt;
    int linhamdtfim;
} MacroNameTable;

typedef struct tabSimItem_s {
    char label[20];
    int endereco;
    char flagTipo = 'X';
} tabSimItem;

// As duas funcoes abaixo (icompare_pred e iequals) https://stackoverflow.com/a/23944175
// Servem para fazer uma comparacao case insensitive de strings
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

int getOp (string token) {
    if (iequals(token,"ADD") == 1) {
        return 1;
    }
    else if (iequals(token,"SUB") == 1) {
        return 2;
    }
    else if (iequals(token,"MULT") == 1) {
        return 3;
    }
    else if (iequals(token,"DIV") == 1) {
        return 4;
    }
    else if (iequals(token,"JMP") == 1) {
        return 5;
    }
    else if (iequals(token,"JMPN") == 1) {
        return 6;
    }
    else if (iequals(token,"JMPP") == 1) {
        return 7;
    }
    else if (iequals(token,"JMPZ") == 1) {
        return 8;
    }
    else if (iequals(token,"COPY") == 1) {
        return 9;
    }
    else if (iequals(token,"LOAD") == 1) {
        return 10;
    }
    else if (iequals(token,"STORE") == 1) {
        return 11;
    }
    else if (iequals(token,"INPUT") == 1) {
        return 12;
    }
    else if (iequals(token,"OUTPUT") == 1) {
        return 13;
    }
    else if (iequals(token,"STOP") == 1) {
        return 14;
    }
    else {
        return -1;
    }
}

int getDiretiva (string token) {
    if (iequals(token,"SPACE") == 1) {
        return 101;
    }
    else if (iequals(token,"CONST") == 1) {
        return 102;
    }
    else {
        return -1;
    }
}

int getTam (int opCode) {
    if (opCode == 9) {
        return 3;
    }
    else if (opCode == 14) {
        return 1;
    }
    else {
        return 2;
    }
}

void printaTabSimTemp (list <tabSimItem> tabSim) {
    string texto;
    list <tabSimItem> :: iterator it;

    it = tabSim.begin();
    while (it != tabSim.end()) {
        texto = it->label;
        cout << "Label: " << texto << "\tEndereco: " << it->endereco << "\tTipo: " << it->flagTipo << endl;
        it++;
    }
}

void printaTemp (list <string> programa) {
    list <string> :: iterator it;

    it = programa.begin();
    while (it != programa.end()) {
        cout << *it << endl;
        it++;
    }
}

int tabSimSeek (list <tabSimItem> tabSim, string token, char* tipo) {
    list <tabSimItem> :: iterator it;

    it = tabSim.begin();
    while (1) {
        if (it == tabSim.end()) {
            if (tipo != NULL) {
                *tipo = 'E';
            }
            return -1;
        }
        else if (iequals(token.c_str(),it->label) != 1) {
            it++;
        }
        else {
            if (tipo != NULL) {
              *tipo = it->flagTipo;
            }
            return it->endereco;
        }
    }
}

//Vitor pls
int tabSimSeek0 (list <MacroNameTable> MNT, string token) {
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

int tabSimSeek2 (list <MacroNameTable> MNT, string token) {
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

// Funcao de pre processamento. Resolve IF EQU e remove comentarios
void preProc (list <tabSimItem> *tabIfEqu, string nomeIN, string nomeOUT){//Aqui tem o token e o token aux, sendo o token aux o  token anterior ao token
	ifstream arquivo;								//Para facilitar quando achar um EQU
    ofstream codprep;								//Quando for fazer a parte de erros tem que mudar um pouqinho	
    string token,linha,tokenaux;					
    int equflag,ifflag,nextlineflag,sectiontextflag,sectiondataflag,flagsimdefinido,contarg;
    int i=0, endereco=0, op=-1, simEndereco=-1,lala,linhacont,contlabel;
    tabSimItem SimAtual;

    arquivo.open(nomeIN);
    codprep.open(nomeOUT);
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
           						simEndereco = tabSimSeek(*tabIfEqu, tokenaux, NULL); //procura na tabela de simbolo
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
       							tabIfEqu->push_back(SimAtual);
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
        			}else if((lala = tabSimSeek(*tabIfEqu, token,NULL)) >= 0 && contarg == 1){
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

void macroProc (list <MacroNameTable> *MNT, string nomeIN, string nomeOUT){ //Um tanto desses inteiros eu copiei da sua parte e tenho medo de apagar
    ifstream arquivo;
    ofstream codprep;
    string token,linha,tokenaux,mdt[100],argmacro,argumentodeclarado[10],argumentochamado[10];
    int equflag,ifflag,nextlineflag,i=0, endereco=0, op=-1, simEndereco=-1,lala,macroflag=0,argumeto;
    int mdtcont=0,contarg=0,mdtsearch = 0, fim,z,macroflag2,endmacroflag,tirarlinha,mdtfim,trocaargumentos,nargumentos;
    MacroNameTable SimAtual;

    arquivo.open(nomeIN);
    codprep.open(nomeOUT);
    while (getline(arquivo,linha)) {
        contarg = 0;
        macroflag2 = 0;
        endmacroflag = 0;
        tirarlinha = 0;
        stringstream linhaStream(linha);
        while(linhaStream >> token){
            //errotoken
            if((mdtsearch = tabSimSeek0(*MNT,token)) > -1){
                mdtfim = tabSimSeek2(*MNT,token);
                nargumentos = tabSimSeek3(*MNT,token);
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
                    if(trocaargumentos == 1){
                        if(mdt[mdtsearch].find(argumentodeclarado[0]) < 1000){
                                mdt[mdtsearch].replace(mdt[mdtsearch].find(argumentodeclarado[0]),max(argumentochamado[0].length(),argumentodeclarado[0].length()),argumentochamado[0]);
                        }
                        if(mdt[mdtsearch].find(argumentodeclarado[1]) < 1000){
                                mdt[mdtsearch].replace(mdt[mdtsearch].find(argumentodeclarado[1]),max(argumentochamado[1].length(),argumentodeclarado[1].length()),argumentochamado[1]);
                        }
                        if(mdt[mdtsearch].find(argumentodeclarado[2]) < 1000){
                                mdt[mdtsearch].replace(mdt[mdtsearch].find(argumentodeclarado[2]),max(argumentochamado[2].length(),argumentodeclarado[2].length()),argumentochamado[2]);
                        }
                        if(mdt[mdtsearch].find(argumentodeclarado[3]) < 1000){
                                mdt[mdtsearch].replace(mdt[mdtsearch].find(argumentodeclarado[3]),max(argumentochamado[3].length(),argumentodeclarado[3].length()),argumentochamado[3]);
                        }
                        if(mdt[mdtsearch].find(argumentodeclarado[0]) < 1000){
                                mdt[mdtsearch].replace(mdt[mdtsearch].find(argumentodeclarado[0]),max(argumentochamado[0].length(),argumentodeclarado[0].length()),argumentochamado[0]);
                        }
                        if(mdt[mdtsearch].find(argumentodeclarado[1]) < 1000){
                                mdt[mdtsearch].replace(mdt[mdtsearch].find(argumentodeclarado[1]),max(argumentochamado[1].length(),argumentodeclarado[1].length()),argumentochamado[1]);
                        }
                        if(mdt[mdtsearch].find(argumentodeclarado[2]) < 1000){
                                mdt[mdtsearch].replace(mdt[mdtsearch].find(argumentodeclarado[2]),max(argumentochamado[2].length(),argumentodeclarado[2].length()),argumentochamado[2]);
                        }
                        if(mdt[mdtsearch].find(argumentodeclarado[3]) < 1000){
                                mdt[mdtsearch].replace(mdt[mdtsearch].find(argumentodeclarado[3]),max(argumentochamado[3].length(),argumentodeclarado[3].length()),argumentochamado[3]);
                        }
                    }

                    codprep << mdt[mdtsearch] << endl;
                }
            }
            if((iequals(token,"MACRO") == 1)){//Verifica se o token e um EQU, se for EQU ele manda o label anterior pra tabela
                macroflag = 1;//Seta uma flag de macro = 1
                macroflag2 = 1;
                if ((tokenaux.back()) == ':') {//Verifica se o token anterior a macro e um label
                    tokenaux.pop_back();
                    simEndereco = tabSimSeek0(*MNT, tokenaux);
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
                                argumentodeclarado[z] = argmacro;
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

void primeiraPassagem (list <tabSimItem> *tabSim, list <string> *programa, string nomeIN) {
    ifstream arquivo;
    char colon, semicolon;
    int tamanho, i=0, endereco=0, op=-1, simEndereco=-1, diretiva=-1, flagSection=0, flagNovaLinha=0, flagLabelDuplo=0;
    string token, linhaIN, linhaOUT;
    tabSimItem SimAtual;

    arquivo.open(nomeIN);
    if (!arquivo) {
        cout << "Erro, nao foi possivel abrir o arquivo " << nomeIN << endl;
        return;
    }
    getline(arquivo,linhaIN);
    stringstream linhaStream(linhaIN);
    linhaStream >> token;
    flagSection = iequals(token,"SECTION");
    linhaStream >> token;
    if ((iequals(token,"TEXT") != 1) || (flagSection != 1)) {
        cout << "Erro, nao tem section text" << endl; //qual erro
    }
    flagSection = 0;
    while (getline(arquivo,linhaIN)) {
        stringstream linhaStream(linhaIN);
        while (linhaStream >> token) {
            if (iequals(token,"SECTION") == 1) {
                flagSection = 1;
                linhaStream >> token;
                if (iequals(token,"DATA") == 1) {
                    goto sectionData;
                }
                else {
                    cout << "Erro sintatico, definicao de secao errada"; //qual erro
                }
            }
            colon = token.back();
            if (colon == ':') {
                if (flagLabelDuplo) {
                    cout << "Erro sintatico, dois rotulos para uma mesma linha: " << token << endl;
                }
                token.pop_back();
                simEndereco = tabSimSeek(*tabSim,token,NULL);
                if (simEndereco > 0) {
                    cout << "Erro semantico, simbolo ja definido: |" << token << endl;
                }
                token.push_back('\0');
                token.copy(SimAtual.label, token.length());
                SimAtual.endereco = endereco;
                tabSim->push_back(SimAtual);
                flagLabelDuplo = 1;
            }
            else {
                flagLabelDuplo = 0;
                op = getOp(token);
                diretiva = getDiretiva(token);
                if (op > 0) {
                    i = 1;
                    tamanho = getTam(op);
                    if (op == 9) { //caso do copy
                        linhaOUT.append(token);
                        linhaOUT.push_back(' ');
                        linhaStream >> token;
                        if (token.find(',') == string::npos) {
                            cout << "Erro sintatico, esperava virgula como separador dos operandos" << endl;
                        }
                        linhaOUT.append(token.substr(0,token.find(',')));
                        linhaOUT.push_back(' ');
                        linhaOUT.append(token.substr(token.find(',')+1,token.length()));
                        programa->push_back(linhaOUT);
                        linhaOUT.clear();
                        endereco+=3;
                        continue;
                    }
                }
                else if (diretiva > 0) {
                    cout << "Erro semantico, SPACE ou CONST na secao TEXT" << endl;
                }
                linhaOUT.append(token);
                linhaOUT.push_back(' ');
                if (i == tamanho) {
                    programa->push_back(linhaOUT);
                    linhaOUT.clear();
                }
                i++;
                endereco++;
            }
        }
    }
    sectionData:;
    flagNovaLinha = 0;
    while (getline(arquivo,linhaIN)) {
        stringstream linhaStream(linhaIN);
        while (linhaStream >> token) {
            colon = token.back();
            if (colon == ':') {
                token.pop_back();
                simEndereco = tabSimSeek(*tabSim,token,NULL);
                if (simEndereco > 0) {
                    cout << "Erro semantico, simbolo ja definido: |" << token << endl;
                }
                token.push_back('\0');
                token.copy(SimAtual.label, token.length());
                SimAtual.endereco = endereco;
                SimAtual.flagTipo = 'D';
                tabSim->push_back(SimAtual);
            }
            else {
                op = getOp(token);
                diretiva = getDiretiva(token);
                if (diretiva > 0) {
                    if (diretiva == 102) { //se for CONST
                        if (flagNovaLinha == 1){
                            programa->push_back(linhaOUT);
                            linhaOUT.clear();
                        }
                        linhaOUT.append(token);
                        linhaOUT.push_back(' ');
                        linhaStream >> token;
                        op = getOp(token);
                        diretiva = getDiretiva(token);
                        if ((op > 0) || (diretiva > 0)) {
                            cout << "Erro sintatico, esperava a definicao da CONST";
                        }
                        if ((stoi(token,NULL) == 0) || (stoi(token,NULL,16) == 0)) {
                            SimAtual.flagTipo = '0';
                        }
                        else {
                            SimAtual.flagTipo = 'C';
                        }
                        tabSim->pop_back();
                        tabSim->push_back(SimAtual);
                        linhaOUT.append(token);
                        programa->push_back(linhaOUT);
                        linhaOUT.clear();
                        endereco++;
                        flagNovaLinha = 0;
                        continue;
                    }
                    else { //se for SPACE
                        if (flagNovaLinha == 1) {
                            programa->push_back(linhaOUT);
                            linhaOUT.clear();
                        }
                        flagNovaLinha = 1;
                        linhaOUT.append(token);
                        linhaOUT.push_back(' ');
                        endereco++;
                    }
                }
                else if (op > 0) {
                    cout << "Erro semantico, instrucao na secao DATA" << endl;
                    flagNovaLinha = 0;
                }
                else {
                    if (flagNovaLinha == 1) {
                        linhaOUT.append(token);
                        programa->push_back(linhaOUT);
                        linhaOUT.clear();
                        endereco += stoi(token,NULL)-1;
                        flagNovaLinha = 0;
                    }
                    else {
                        cout << "Erro sintatico, esperava CONST ou SPACE" << endl; //qual erro
                    }
                }                
            }
        }
    }
    if (linhaOUT.length() != 0) {
        programa->push_back(linhaOUT);
        linhaOUT.clear();
    }
    arquivo.close();
}

void scanner (string token) {
    int i=0, flagCharInv=0;

    if ((!isalpha(token[0])) && token[0] != '_') {
        cout << "Erro lexico, primeiro caracter deve ser uma letra ou underscore: " << token << endl;
    }
    if (token.length() > 20) {
        cout << "Erro lexico, token muito longo: " << token << endl;
    }
    while (i < (token.length())) {
        if (!isalnum(token[i])) {
            if (token[i] != '_'){
                flagCharInv = 1;
            }
        }
        i++;
    }
    if (flagCharInv) {
        cout << "Erro lexico, token possui caracteres invalidos: " << token << endl;
    }
}

// Assumindo que está sem labels e sem comentários
void segundaPassagem (list <tabSimItem> tabSim, list <string> programa, string nomeOUT) {
    ofstream output;
    int i, op=-1, tamanho=-1, simEndereco=-1, diretiva=-1, flag=0;
    char tipo='Z';
    string token;
    list <string> :: iterator it;

    output.open(nomeOUT);
    if (!output) {
        cout << "Erro, nao foi possivel criar o arquivo de saida" << endl;
        return;
    }
    it = programa.begin();
    while (it != programa.end()) {
        if (it->length() == 0) {
            continue;
        }
        stringstream linhaStream(*it);
        linhaStream >> token;               // Primeiro token da linha, deve obrigatoriamente ser operacao?
        scanner(token);
        op = getOp(token);
        diretiva = getDiretiva(token);
        if (diretiva < 0) {
            tamanho = getTam(op);
            if (op < 0) {
                cout << "Erro sintatico, operacao invalida: |" << token << "| na linha : ``"  << "``" << endl;
            }
            else {
                output << op << " "; // Escreve no arquivo o opcode
                i=1;
                while (linhaStream >> token) {
                    scanner(token);
                    if (token.find('+') == string::npos) {
                        simEndereco = tabSimSeek(tabSim,token,&tipo);
                        if (simEndereco < 0) {
                            cout << "Erro semantico, simbolo nao definido: |" << token << "| na linha : ``"  << "``" << endl; //qual erro
                        }
                        else {
                            output << simEndereco << " ";
                        }
                    }
                    else {
                        simEndereco = tabSimSeek(tabSim, token.substr(0,token.find('+')),&tipo);
                        if (simEndereco < 0) {
                            cout << "Erro semantico, simbolo nao definido: |" << token.substr(0,token.find('+')) << "| na linha : ``"  << "``" << endl; //qual erro
                        }
                        else {
                            output << simEndereco+stoi(token.substr(token.find('+')+1,token.length()))<< " ";
                        }
                    }
                    if ((op == 6) || (op == 7) || (op == 8)) { //jumps
                        if ((tipo == 'C') || (tipo == '0') || (tipo == 'D')) {
                            cout << "Erro semantico, pulo para secao data" << endl;
                        }
                    }
                    else if (op == 4) { //div
                        if (tipo == '0') {
                            cout << "Erro semantico, divisao por 0" << endl;
                        }
                    }
                    else if ((op == 11) || (op == 12)) { //store e input
                        if ((tipo == 'C') || (tipo == '0')) {
                            cout << "Erro semantico, tentativa de modificar valor constante" << endl;
                        }
                        else if (tipo == 'X') {
                            cout << "Erro semantico, argumento invalido (label nao eh do tipo dado)" << endl;
                        }
                    }
                    else if (op == 9) { //copy
                        if (flag) { //pegando o segundo argumento (destino)
                            if ((tipo == 'C') || (tipo == '0')) {
                                cout << "Erro semantico, tentativa de modificar valor constante" << endl;
                            }
                            else if (tipo == 'X') {
                                cout << "Erro semantico, argumento invalido (label nao eh do tipo dado)" << endl;
                            }
                            flag = 0;
                        }
                        else {
                            flag = 1;
                        }
                        
                    }                    
                    i++;
                }
                if (i != tamanho) {
                    cout << "Erro sintatico, numero de operandos errado: |" << token << "| na linha : ``" <<  "``" << endl;
                }
            }
        }
        else {
            if (diretiva == 102) { //const
                linhaStream >> token;
                if ((token.substr(0,2) == "0x") || (token.substr(1,2) == "0x")) { //caso do -0x...
                    output << stoi(token,NULL,16) << " ";
                }
                else {
                    output << stoi(token,NULL) << " ";
                }
            }
            else { //space
                output << 0 << " ";
                if (linhaStream >> token) {
                    i = 1;
                    while (i < stoi(token,NULL)) {
                        output << 0 << " ";
                        i++;
                    }
                }
            }
        }
        it++;
    }
}

int main (int argc, char* argv[]) {
    list <tabSimItem> tabSim, tabIfEqu;
    list <string> programa;
	list <MacroNameTable> MNT;
    string nomeIN, nomeOUT;

    if (argc < 4) {
        cout <<  "Erro - argumentos insuficientes" << endl;
        return 0;
    }
    else {
        if (strcmp(argv[1],"-o") && strcmp(argv[1],"-m") && strcmp(argv[1],"-p")) {
            cout <<  "Erro - argumentos fora de ordem ou invalidos" << endl;
            return 0;
        }
        nomeIN.assign(argv[2]);
        nomeIN.append(".asm");
        nomeOUT.assign(argv[3]);
		if (!strcmp(argv[1],"-p")) {
			nomeOUT.append(".pre");
			//cout << nomeIN << " " << nomeOUT << endl;
			preProc(&tabIfEqu,nomeIN,nomeOUT);
		}
		else if (!strcmp(argv[1],"-m")) {
			nomeOUT.append(".mcr");
			//cout << nomeIN << " " << nomeOUT << endl;
			preProc(&tabIfEqu,nomeIN,"codPre.txt");
			macroProc(&MNT,"codPre.txt", nomeOUT);
			remove("codPre.txt");
		}
		else if (!strcmp(argv[1],"-o")) {
			nomeOUT.append(".o");
			//cout << nomeIN << " " << nomeOUT << endl;
			preProc(&tabIfEqu,nomeIN,"codPre.txt");
			macroProc(&MNT,"codPre.txt", "codMacro.txt");
			primeiraPassagem(&tabSim,&programa,"codMacro.txt");
			segundaPassagem(tabSim,programa,nomeOUT);
			remove("codPre.txt");
			remove("codMacro.txt");
		}
    }
    //printaTabSimTemp(tabSim);

    return 0;
}