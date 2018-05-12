/*
• Remover toda a case-sensitividade, mesmo na tabela de símbolos, então usar iequals para comparar string
- Aparentemente feito. Foi só usar iequals na tabela de símbolos em vez de strcmp

• Desconsiderar tabula¸c˜oes, quebras de linha e espa¸cos desnecess´arios (incluindo quebra de linha depois do r´otulo).
- Não sei se acontece isso (especialmente quebra de linha dps do rótulo). Verificar dps e fazer mudanças necessárias
    - Agora ele lida com quebra de linha depois do rótulo. O que acontecia é que a primeira passagem gerava o arquivo (codPreProcessado) e quando tinha quebra
    de linha dps do rótulo ele gerava linha em branco, aí na 2 passagem ele lia a linha e interpretava errado. Foi necessário adicionar um teste para ver se 
    a linha lida tem tamanho 0.
    - Agora está perfeito. Na 1 passagem, ele le token por token (em vez de ler linhas). Desse modo, pode ser até 1 token por linha que ele não fica perdido

• O comando COPY deve utilizar uma v´ırgula entre os operandos SEM ESPAC¸ O (COPY A,B)
- Muito ridículo isso aqui. Vai precisar modificar o código. Se não me engano para macro vai ser assim tb.

• A diretiva CONST deve aceitar declara¸c˜ao em hexadecimal tamb´em (no formato 0x00);
- Por enquanto nenhuma diretiva está implementada, mas quando for fazer isso levar em conta.

• Identificar erros durante a montagem.
- Pegar a lista no roteiro, mas deixa pra se preocupar com isso dps.

• Macro é declarada na seção texto e EQU vem antes de tudo.

• Verificar, na primeira passagem, se o rótulo existe na tabela de símbolos. Se existe, dar erro de símbolo redefinido.


git pull
git add trab.cpp
git commit -m "mensagem"
git push
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

void inputTemp (string *nome) {
    cout << "Digite o nome do arquivo \n";
    cin >> *nome;
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
            //break; - quase ctz de que n preciso disso
        }
        else if (iequals(token.c_str(),it->label) != 1) {
            it++;
        }
        else {
            return it->endereco;
            //break; - quase ctz de que n preciso disso
        }
    }
}

// Aqui tem que adequar para as diretivas e áreas .text e .data
// Ele ta supondo que ou é label, ou instrução ou parametro
// Por enquanto ele está apenas fazendo a tabela de símbolos
// Gera um arquivo intermediário sem rótulos e comentários
void primeiraPassagem (list <tabSimItem> *tabSim, string nome) {
    ifstream arquivo;
    ofstream codPreP;
    char colon, semicolon;
    int tamanho, i=0, endereco=0, op=-1;
    string token, linha;
    tabSimItem SimAtual;

    arquivo.open(nome); //O nome do arquivo vai ser passado pelo terminal, então não sei ainda como vai fazer
    codPreP.open("codPreProcessado.txt");
    // Procurar por SECTION TEXT aqui?
    while (arquivo >> token) {
        colon = token.back();
        if (colon == ':') {
            //Procurar na tabela de símbolos
            token.pop_back();
            token.push_back('\0');
            token.copy(SimAtual.label, token.length());
            SimAtual.endereco = endereco;
            tabSim->push_back(SimAtual);
        }
        else {
            op = getOp(token);
            if (op > 0) {
                i=1;
                tamanho = getTam(op);
            }
            codPreP << token << " ";
            if (i >= tamanho) {
                codPreP << endl;
            }
            i++;
            endereco++;
        }
    }
    arquivo.close();
    codPreP.close();
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

// WIP
// Assumindo que está sem labels e sem comentários
void segundaPassagem (list <tabSimItem> tabSim,string nome) {
    ifstream arquivo;
    ofstream output;
    int i, op=-1, tamanho=-1, enderecoSim=-1;
    string token, linha;

    arquivo.open("codPreProcessado.txt"); //O nome do arquivo vai ser passado pelo terminal, então não sei ainda como vai fazer
    output.open("aout.txt");
    while (getline(arquivo,linha)) {
        if (linha.length() == 0) {
            continue;
        }
        stringstream linhaStream(linha);
        linhaStream >> token;               // Primeiro token da linha, deve obrigatoriamente ser operacao?
        //scanner(token);
        op = getOp(token);
        tamanho = getTam(op);
        if (op < 0) {
            cout << "Erro, operacao invalida: |" << token << "| na linha : ``" << linha << "``" << endl;
        }
        else {
            output << op << " "; // Escreve no arquivo o opcode
            i=1;
            while (linhaStream >> token) {
                //scanner(token);
                enderecoSim = tabSimSeek(tabSim, token);
                if (enderecoSim < 0) {
                    cout << "Erro, simbolo nao definido: |" << token << "| na linha : ``" << linha << "``" << endl;
                }
                else {
                    output << enderecoSim << " ";
                }
                i++;
            }
            if (i != tamanho) {
                cout << "Erro, numero de operandos diferente da operacao: |" << token << "| na linha : ``" << linha << "``" << endl;
            }
        }
    }
}

int main () {
    list <tabSimItem> tabSim;
    string nome="teste.asm";

    //inputTemp (&nome);
    primeiraPassagem(&tabSim,nome);
    printaTabSimTemp(tabSim);
    segundaPassagem(tabSim,nome);
    return 0;
}