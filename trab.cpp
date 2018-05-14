/*
• Identificar erros durante a montagem.
- Pegar a lista no roteiro, mas deixa pra se preocupar com isso dps.

• Macro é declarada na seção texto e EQU vem antes de tudo.

• Tem que verificar erro se não consegue abrir os arquivos

• A primeira passagem está mais complexa do que precisaria ser, mas isso tira complexidade da segunda.

--------------------------------------VERSAO ATUAL--------------------------------------
* Aceita o const hexadecimal
* Faz a operação do LABEL+X (sem espaço antes e depois do +)
* Aceita parametros da linha de comando
* Remove o arquivo intermediario
----------------------------------------------------------------------------------------
*/
#include <iostream>
#include <fstream>                           //Para lidar com o arquivo. Nunca usei isso, então não sei comofas
#include <string.h>
#include <list>
#include <ctype.h>                          // Usando na função isalpha
#include <sstream>                          // Para retirar as palavras da linha
#include <algorithm>                        // Para procurar na tabela de simbolos

using namespace std;

typedef struct tabSimItem_s {
    char label[20];
    int endereco;
} tabSimItem;

// As duas funcoes abaixo https://stackoverflow.com/a/23944175
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

// Gera um arquivo intermediário sem rótulos
void primeiraPassagem (list <tabSimItem> *tabSim, string nome) {
    ifstream arquivo;
    ofstream codInt;
    char colon, semicolon;
    int tamanho, i=0, endereco=0, op=-1, simEndereco=-1, diretiva=-1, flag=0;
    string token;
    tabSimItem SimAtual;

    arquivo.open(nome); //O nome do arquivo vai ser passado pelo terminal, então não sei ainda como vai fazer
    codInt.open("codInt.txt");
    arquivo >> token;
    flag = iequals(token,"SECTION");
    arquivo >> token;
    if ((iequals(token,"TEXT") != 1) || (flag != 1)) {
        cout << "Erro, não tem section text" << endl;
    }
    flag = 0;
    while (arquivo >> token) {
        if (iequals(token,"SECTION") == 1) {
            flag = 1;
            continue;
        }
        if (iequals(token,"DATA") == 1) {
            if (flag == 1) {
                break;
            }
            else {
                cout << "Erro, definicao de secao errada";
            }
        }
        colon = token.back();
        if (colon == ':') {
            token.pop_back();
            simEndereco = tabSimSeek(*tabSim, token);
            if (simEndereco > 0) {
                cout << "Erro, simbolo ja definido: |" << token << endl;
            }
            token.push_back('\0');
            token.copy(SimAtual.label, token.length());
            SimAtual.endereco = endereco;
            tabSim->push_back(SimAtual);
        }
        else {
            op = getOp(token);
            diretiva = getDiretiva(token);
            if (op > 0) {
                i = 1;
                tamanho = getTam(op);
                if (op == 9) { //caso do copy
                    codInt << token << " ";
                    arquivo >> token;
                    if (token.find(',') == string::npos) {
                        cout << "Erro, onde esta a virgula" << endl;
                    }
                    codInt << token.substr(0,token.find(',')) << " " << token.substr(token.find(',')+1,token.length()) << endl;
                    endereco+=3;
                    continue;
                }
            }
            else if (diretiva > 0) {
                cout << "Erro, SPACE ou CONST na secao errada" << endl;
            }
            codInt << token << " ";
            if (i == tamanho) {
                codInt << endl;
            }
            i++;
            endereco++;
        }
    }
    if (flag == 1) {
        flag = 0;
        while (arquivo >> token) {
            colon = token.back();
            if (colon == ':') {
                token.pop_back();
                simEndereco = tabSimSeek(*tabSim, token);
                if (simEndereco > 0) {
                    cout << "Erro, simbolo ja definido: |" << token << endl;
                }
                token.push_back('\0');
                token.copy(SimAtual.label, token.length());
                SimAtual.endereco = endereco;
                tabSim->push_back(SimAtual);
            }
            else {
                op = getOp(token);
                diretiva = getDiretiva(token);
                if (diretiva > 0) {
                    if (diretiva == 102) { //se for CONST
                        if (flag == 1){
                            codInt << endl;
                        }
                        codInt << token << " ";
                        arquivo >> token;
                        op = getOp(token);
                        diretiva = getDiretiva(token);
                        if ((op > 0) || (diretiva > 0)) {
                            cout << "Erro, esperava a definicao da CONST";
                        }
                        codInt << token << endl;
                        endereco++;
                        flag = 0;
                        continue;
                    }
                    else { //se for SPACE
                        if (flag == 1) {
                            codInt << endl;
                        }
                        flag = 1;
                        codInt << token << " ";
                        endereco++;
                    }
                }
                else if (op > 0) {
                    cout << "Erro, operacao na secao errada" << endl;
                    flag = 0;
                }
                else {
                    if (flag == 1) {
                        codInt << token << endl;
                        endereco += stoi(token,NULL)-1;
                        flag = 0;
                    }
                    else {
                        cout << "Erro, esperava CONST ou SPACE" << endl;
                    }
                }                
            }
        }
    }
    arquivo.close();
    codInt.close();
}

// Checar tamanho do token??
// Quais caracteres especiais pode ter?
// Dar um jeito de parar tudo quando der erro e printar warning
void scanner (string token) {
    /* Os identificadores de vari´aveis e r´otulos s˜ao limitados em 20 caracteres e seguem
as regras comuns da linguagem C, sendo compostos por letras, n´umeros ou o caractere
(underscore) e com a restri¸c˜ao de que o primeiro caractere n˜ao pode ser um n´umero.*/
    if (!isalpha(token[0])) {
        // PARA TUDO, ERRO LÉXICO
    }
    else {

    }
}

// Assumindo que está sem labels e sem comentários
void segundaPassagem (list <tabSimItem> tabSim,string nomeOUT) {
    ifstream arquivo;
    ofstream output;
    int i, op=-1, tamanho=-1, simEndereco=-1, diretiva=-1;
    string token, linha;

    arquivo.open("codInt.txt"); //O nome do arquivo vai ser passado pelo terminal, então não sei ainda como vai fazer
    output.open(nomeOUT);
    while (getline(arquivo,linha)) {
        if (linha.length() == 0) {
            continue;
        }
        stringstream linhaStream(linha);
        linhaStream >> token;               // Primeiro token da linha, deve obrigatoriamente ser operacao?
        //scanner(token);
        op = getOp(token);
        diretiva = getDiretiva(token);
        if (diretiva < 0) {
            tamanho = getTam(op);
            if (op < 0) {
                cout << "Erro, operacao invalida: |" << token << "| na linha : ``" << linha << "``" << endl;
            }
            else {
                output << op << " "; // Escreve no arquivo o opcode
                i=1;
                while (linhaStream >> token) {
                    //scanner(token);
                    if (token.find('+') == string::npos) {
                        simEndereco = tabSimSeek(tabSim, token);
                        if (simEndereco < 0) {
                            cout << "Erro, simbolo nao definido: |" << token << "| na linha : ``" << linha << "``" << endl;
                        }
                        else {
                            output << simEndereco << " ";
                        }
                    }
                    else {
                        simEndereco = tabSimSeek(tabSim, token.substr(0,token.find('+')));
                        if (simEndereco < 0) {
                            cout << "Erro, simbolo nao definido: |" << token.substr(0,token.find('+')) << "| na linha : ``" << linha << "``" << endl;
                        }
                        else {
                            output << simEndereco+stoi(token.substr(token.find('+')+1,token.length()))<< " ";
                        }
                    }                    
                    i++;
                }
                if (i != tamanho) {
                    cout << "Erro, numero de operandos diferente da operacao: |" << token << "| na linha : ``" << linha << "``" << endl;
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
                    while (i < stoi(token)) {
                        output << 0 << " ";
                        i++;
                    }
                }
            }
        }
    }
}

int main (int argc, char* argv[]) {
    list <tabSimItem> tabSim;
    string nomeIN, nomeOUT;
    if (argc < 4) {
        cout <<  "Erro - parametros insuficientes, usando parametros padrao" << endl;
        nomeIN = "bin.asm";
        nomeOUT = "aout.txt";
    }
    else {
        if (strcmp(argv[1],"-o") && strcmp(argv[1],"-m") && strcmp(argv[1],"-p")) {
            cout <<  "Erro - argumentos fora de ordem" << endl;
            return 0;
        }
        nomeIN.assign(argv[2]);
        nomeIN.append(".asm");
        nomeOUT.assign(argv[3]);
        nomeOUT.append(".txt"); //aqui vai depender da operacao
    }
    cout << nomeIN << " " << nomeOUT << endl;
    //inputTemp (&nome);
    primeiraPassagem(&tabSim,nomeIN);
    printaTabSimTemp(tabSim);
    segundaPassagem(tabSim,nomeOUT);
    remove ("codInt.txt");
    return 0;
}