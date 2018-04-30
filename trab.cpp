/*
Primeira passagem:  *Faz a tabela de Símbolos






*/
#include <iostream>
#include<fstream>                           //Para lidar com o arquivo. Nunca usei isso, então não sei comofas


using namespace std;


void carregaArquivo (FILE *fp, char *buffer) {                   //Ver se a passagem dos argumentos está correta
    ifstream arquivo;
    int tamanho;

    arquivo.open("nomeDoArquivo.whatever"); //O nome do arquivo vai ser passado pelo terminal, então não sei ainda como vai fazer
    arquivo.seekg(0, arquivo.end);    // go to the end
    tamanho = .tellg();           // report location (this is the length)
    arquivo.seekg(0, arquivo.beg);    // go back to the beginning
    //buffer = new char[tamanho];    // allocate memory for a buffer of appropriate dimension
    buffer = (char*) malloc(tamanho*sizeof(char));  //será que isso da certo?
    arquivo.read(buffer, tamanho);       // read the whole file into the buffer
    arquivo.close();   

    //agora o arquivo foi carregado em "buffer"
}

void primeiraPassagem (char *buffer, char *limpo) {       //Esse limpo vai ser o resultado da primeira passagem, sem rotulos e comentarios e diretivas etc
    int i=0;

    while (buffer[i]!=NULL) {

    }
}
/*
Isso tudo deve estar muito errado e deve ter um jeito mais fácil. Vou pesquisar antes de fazer.
*/