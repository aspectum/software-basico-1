/*
Primeira passagem:  *Faz a tabela de Símbolos






*/
#include <iostream>
#include <fstream>                           //Para lidar com o arquivo. Nunca usei isso, então não sei comofas
#include <string>
#include <list>


using namespace std;

typedef struct tabSimItem_s {
    char label[20];
    int endereco;
} tabSimItem;

int getLength (char *token) {
    if (*token == "ADD") {
        return 2;
    }
    else if (*token == "SUB") {
        return 2;
    }
    else if (*token == "MULT") {
        return 2;
    }
    else if (*token == "DIV") {
        return 2;
    }
    else if (*token == "JMP") {
        return 2;
    }
    else if (*token == "JMPN") {
        return 2;
    }
    else if (*token == "JMPP") {
        return 2;
    }
    else if (*token == "JMPZ") {
        return 2;
    }
    else if (*token == "COPY") {
        return 3;
    }
    else if (*token == "LOAD") {
        return 2;
    }
    else if (*token == "STORE") {
        return 2;
    }
    else if (*token == "INPUT") {
        return 2;
    }
    else if (*token == "OUTPUT") {
        return 2;
    }
    else if (*token == "STOP") {
        return 1;
    }
    else {
        return 0;
    }
    
}

// Aqui tem que adequar para as diretivas e áreas .text e .data
// Ele ta supondo que ou é label, ou instrução ou parametro
void primeiraPassagem (list <tabSimItem> *tabSim) {
    ifstream arquivo;
    int tamanho, i=0, endereco=0;
    string token;
    tabSimItem SimAtual;

    arquivo.open("nomeDoArquivo.whatever"); //O nome do arquivo vai ser passado pelo terminal, então não sei ainda como vai fazer
    while (arquivo >> token) {
        if token[token.length()] == ':' {
            token.pop_back();
            SimAtual.label = token;
            SimAtual.endereco = endereco;
            *tabSim.push_back(SimAtual);
        }
        else {
            tamanho = getLength();
            endereco += tamanho;
            for (i = tamanho, i > 0, i--) {// Isso ta fazendo o i-- quando? Pode ser que dê treta. Fazer verificação de erro dps
                arquivo >> token;
            }
        }
    }
}

int main () {
    list <tabSimItem> tabSim;

    primeiraPassagem(&tabSim);
    return 0;
}