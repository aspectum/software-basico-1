/*
Está declarando token como string sem o tamanho. Deve dar pau. Talvez usar char* token
olaosj[fasmkldnas[dj]]





*/
#include <iostream>
#include <fstream>                           //Para lidar com o arquivo. Nunca usei isso, então não sei comofas
#include <string.h>
#include <list>
#include <ctype.h>                          // Usando na função isalpha
#include <sstream>                          // Para retirar as palavras da linha

using namespace std;

typedef struct tabSimItem_s {
    char label[20];
    int endereco;
} tabSimItem;

int getLength (string token) {
    if (token == "ADD") {
        return 2;
    }
    else if (token == "SUB") {
        return 2;
    }
    else if (token == "MULT") {
        return 2;
    }
    else if (token == "DIV") {
        return 2;
    }
    else if (token == "JMP") {
        return 2;
    }
    else if (token == "JMPN") {
        return 2;
    }
    else if (token == "JMPP") {
        return 2;
    }
    else if (token == "JMPZ") {
        return 2;
    }
    else if (token == "COPY") {
        return 3;
    }
    else if (token == "LOAD") {
        return 2;
    }
    else if (token == "STORE") {
        return 2;
    }
    else if (token == "INPUT") {
        return 2;
    }
    else if (token == "OUTPUT") {
        return 2;
    }
    else if (token == "STOP") {
        return 1;
    }
    else {
        return 0;
    }
    
}

// Aqui tem que adequar para as diretivas e áreas .text e .data
// Ele ta supondo que ou é label, ou instrução ou parametro
// Por enquanto ele está apenas fazendo a tabela de símbolos
// Gera um arquivo intermediário sem rótulos e comentários
void primeiraPassagem (list <tabSimItem> *tabSim) {
    ifstream arquivo;
    ofstream codPreP;
    int tamanho, i=0, endereco=0;
    string token, linha;
    tabSimItem SimAtual;

    arquivo.open("nomeDoArquivo.whatever"); //O nome do arquivo vai ser passado pelo terminal, então não sei ainda como vai fazer
    codPreP.open("codPreProcessado.txt");
    while (getline(arquivo,linha)) {
        stringstream linhaStream(linha);
        while (linhaStream >> token) {
            if (token[token.length()] == ':') {
                token.pop_back();
                token.copy(SimAtual.label, token.length);
                SimAtual.endereco = endereco;
                tabSim->push_back(SimAtual);
            }
            else if (token[0] == ';') {
                break;                          // Quando chega no comentário vai para a próxima linha sem ler mais nada nessa
            }
            else {
                tamanho = getLength(token);
                endereco += tamanho;
                codPreP << token;
                for (i = tamanho; i > 0; i--) {// Isso ta fazendo o i-- quando? Pode ser que dê treta. Fazer verificação de erro dps
                    linhaStream >> token;
                    codPreP << token;
                }
            }
        }
        codPreP << "\n";
    }
}

void scanner (char *token) {
    if (!isalpha(token[0])) {
        // PARA TUDO, ERRO LÉXICO
    }
}

// WIP
void segundaPassagem (list <tabSimItem> tabSim) {
    ifstream arquivo;
    ofstream output;
    int tamanho, i=0, endereco=0;
    string token, linha;

    arquivo.open("nomeDoArquivo.whatever"); //O nome do arquivo vai ser passado pelo terminal, então não sei ainda como vai fazer
    while (getline(arquivo,linha)) {
        while() 
    }
}

int main () {
    list <tabSimItem> tabSim;

    primeiraPassagem(&tabSim);
    return 0;
}