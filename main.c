//
// Created by Henrique Barros on 20/03/2024.
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

unsigned char memoria[154]; //enderecos da memoria
unsigned int mbr; //ocupar toda memoria
unsigned int imm; //23 bits
unsigned int mar = 0; //23 bits
unsigned int pc = 0;
unsigned char e, l, g; //flags
unsigned char ir; //5 bits
unsigned char ro0; //4 bits
unsigned char ro1; //4 bits
unsigned char ro2; //4 bits
//registradores
unsigned int reg[16];
//Ponteiro de leitura do arquivo .txt
unsigned char *pt;

/**
 * Aplica mascara e desloca bits a direita para pegar de 8 em 8 bits todos os bits
 * da variavel 32 bits 'valor' e atribuir em 4 posições na memoria RAM a partir da posicao
 * definida em 'posInicial'
 * @param posInicial posicao inicial da memoria que sera preenchida ela e as 3 posteriores
 * @param valor valor inteiro sem sinal 32 bits para ser armazenado na memoria (vetor de char 8 bits)
 */
void guardarValorParaMemoria(unsigned int posInicial, unsigned int valor){
    //pega de 8 em 8 os bits da variavel valor (32 bits) e desloca para o inicio para ser convertido em char e armazenado em 4 posicoes
    memoria[posInicial++] = (valor & 0xff000000) >> 24;
    memoria[posInicial++] = (valor & 0x00ff0000) >> 16;
    memoria[posInicial++] = (valor & 0x0000ff00) >> 8;
    memoria[posInicial]   = (valor & 0x000000ff);
}

void colocarInstru(int mem){
    unsigned char *ponteiro, rg0, mine[8]; // aux recebe rg0
    unsigned int count = 0, imediatoMem, palavra;

    ponteiro = strtok(pt," ,");//divide a palavra de instrucao em pedacos divididos por , ou espaco
    while(ponteiro){
        if(count == 0){
            strcpy(mine,ponteiro);
        }
        if (count== 1){
            rg0 = (int)strtol(ponteiro,NULL,16);
        }
        if(count == 2){
            imediatoMem = (int)strtol(ponteiro,NULL,16);
        }
        ponteiro = strtok(NULL," ,r");
        count++;
    }
    if(strcmp(mine,"nop")== 0) {
        palavra = 1;//adiciona para a palavra o opcode do comando nop (1)
        palavra = palavra << 24;//desloca o opcode para os 8 bits mais significativos deixando o resto com 0
        guardarValorParaMemoria(mem, palavra);//guarda na memoria a palavra 32 bits
    } else if(strcmp(mine,"add")== 0){
        palavra = 2;
        palavra = (palavra << 3) |rg0;
        palavra = (palavra << 3) | imediatoMem;
        palavra =  palavra << 18;
        guardarValorParaMemoria(mem,palavra);
    } else if(strcmp(mine,"sub")== 0) {
        palavra = 3;
        palavra = (palavra << 3) | rg0;
        palavra = (palavra << 3) | imediatoMem;
        palavra = palavra << 18;
        guardarValorParaMemoria(mem, palavra);
    } else if(strcmp(mine,"mul")== 0) {
        palavra = 4;
        palavra = (palavra << 3) | rg0;
        palavra = (palavra << 3) | imediatoMem;
        palavra = palavra << 18;
        guardarValorParaMemoria(mem, palavra);
    } else if(strcmp(mine,"div")== 0) {
        palavra = 5;
        palavra = (palavra << 3) | rg0;
        palavra = (palavra << 3) | imediatoMem;
        palavra = palavra << 18;
        guardarValorParaMemoria(mem, palavra);
    } else if(strcmp(mine,"cmp")== 0) {
        palavra = 6;
        palavra = (palavra << 3) | rg0;
        palavra = (palavra << 3) | imediatoMem;
        palavra = palavra << 18;
        guardarValorParaMemoria(mem, palavra);
    } else if(strcmp(mine,"movr")== 0) {
        palavra = 7;
        palavra = (palavra << 3) | rg0;
        palavra = (palavra << 3) | imediatoMem;
        palavra = palavra << 18;
        guardarValorParaMemoria(mem, palavra);
    } else if(strcmp(mine,"and")== 0) {
        palavra = 8;
        palavra = (palavra << 3) | rg0;
        palavra = (palavra << 3) | imediatoMem;
        palavra = palavra << 18;
        guardarValorParaMemoria(mem, palavra);
    } else if(strcmp(mine,"or")== 0) {
        palavra = 9;
        palavra = (palavra << 3) | rg0;
        palavra = (palavra << 3) | imediatoMem;
        palavra = palavra << 18;
        guardarValorParaMemoria(mem, palavra);
    } else if(strcmp(mine,"xor")== 0) {
        palavra = 10;
        palavra = (palavra << 3) | rg0;
        palavra = (palavra << 3) | imediatoMem;
        palavra = palavra << 18;
        guardarValorParaMemoria(mem, palavra);
    } else if(strcmp(mine,"not")== 0) {
        palavra = 11;
        palavra = ((palavra << 3)|rg0)<<21;
        guardarValorParaMemoria(mem, palavra);
    } else if(strcmp(mine,"je")== 0) {
        palavra = 12;
        palavra = (palavra << 24)| imediatoMem;
        guardarValorParaMemoria(mem, palavra);
    } else if(strcmp(mine,"jne")== 0) {
        palavra = 13;
        palavra = (palavra << 24)| imediatoMem;
        guardarValorParaMemoria(mem, palavra);
    } else if(strcmp(mine,"jl")== 0) {
        palavra = 14;
        palavra = (palavra << 24)| imediatoMem;
        guardarValorParaMemoria(mem, palavra);
    } else if(strcmp(mine,"jle")== 0) {
        palavra = 15;
        palavra = (palavra << 24)| imediatoMem;
        guardarValorParaMemoria(mem, palavra);
    } else if(strcmp(mine,"jg")== 0) {
        palavra = 16;
        palavra = (palavra << 24)| imediatoMem;
        guardarValorParaMemoria(mem, palavra);
    } else if(strcmp(mine,"jge")== 0) {
        palavra = 17;
        palavra = (palavra << 24)| imediatoMem;
        guardarValorParaMemoria(mem, palavra);
    } else if(strcmp(mine,"jmp")== 0) {
        palavra = 18;
        palavra = (palavra << 24)| imediatoMem;
        guardarValorParaMemoria(mem, palavra);
    } else if(strcmp(mine,"ld")== 0){
        palavra = 19;
        palavra = (palavra << 3) | rg0;
        palavra = (palavra << 21)| imediatoMem;
        guardarValorParaMemoria(mem,palavra);
    } else if(strcmp(mine,"st")== 0){
        palavra = 20;
        palavra = (palavra << 3) | rg0;
        palavra = (palavra << 21)| imediatoMem;
        guardarValorParaMemoria(mem,palavra);
    } else if(strcmp(mine,"movi")== 0) {
        palavra = 21;
        palavra = (palavra << 3) | rg0;
        palavra = (palavra << 21) | imediatoMem;
        guardarValorParaMemoria(mem, palavra);
    } else if(strcmp(mine,"addi")== 0) {
        palavra = 22;
        palavra = (palavra << 3) | rg0;
        palavra = (palavra << 21) | imediatoMem;
        guardarValorParaMemoria(mem, palavra);
    } else if(strcmp(mine,"subi")== 0) {
        palavra = 23;
        palavra = (palavra << 3) | rg0;
        palavra = (palavra << 21) | imediatoMem;
        guardarValorParaMemoria(mem, palavra);
    } else if(strcmp(mine,"muli")== 0) {
        palavra = 24;
        palavra = (palavra << 3) | rg0;
        palavra = (palavra << 21) | imediatoMem;
        guardarValorParaMemoria(mem, palavra);
    } else if(strcmp(mine,"divi")== 0) {
        palavra = 25;
        palavra = (palavra << 3) | rg0;
        palavra = (palavra << 21) | imediatoMem;
        guardarValorParaMemoria(mem, palavra);
    } else if(strcmp(mine,"lsh")== 0) {
        palavra = 26;
        palavra = (palavra << 3) | rg0;
        palavra = (palavra << 21) | imediatoMem;
        guardarValorParaMemoria(mem, palavra);
    } else if(strcmp(mine,"rsh")== 0) {
        palavra = 27;
        palavra = (palavra << 3) | rg0;
        palavra = (palavra << 21) | imediatoMem;
        guardarValorParaMemoria(mem, palavra);
    } else{
        palavra  = 0;
        guardarValorParaMemoria(mem,palavra);
    }
}

/**
 * Busca pelo arquivo instrucoes.txt no mesmo diretorio do executavel e
 */
void lerArquivo(){
    unsigned int index;
    unsigned int valor;
    FILE *arq;
    unsigned char tipo, linha[99];
    unsigned int count = 0;
    arq = fopen("instrucoes.txt", "r");

    while (fgets(linha, sizeof(linha), arq) != NULL){// carrega linha por linha o conteudo do arquivo de instruções na variavel linha
        pt = strtok(linha, "; ");// quebra a linha em pedaços separando por espaços ou ;
        index = (int)strtol(pt,NULL,16);// converte de hexadecimal para inteiro o valor do primeiro pedaço da linha (corresponde ao endereço da memoria)
        while(pt){// itera sobre os pedaços de uma linha
            if(count == 1){//se contador 1 é o pedaço que contem o tipo do comando se é instrução (i) ou dado (d)
                tipo = *pt;
            }else if(count == 2){//se contador 2 contem ou o valor pra ser guardado na memoria ou a instrucao para ser armazenado na memoria
                if(tipo == 'd'){//verifica se o tipo do comando desta linha é de dado, caso seja deve armazenar na memoria
                    valor = (unsigned int)strtol(pt,NULL,16);//converte para inteiro o valor em hexadecimal contido terceira parte da linha de comando caso seja linha de dado
                    guardarValorParaMemoria(index, valor);
                }else{// se esse comando for do tipo 'i' deve armazenar a instrucao contido no ponteiro 'pt' na posicao de memoria 'index'
                    colocarInstru(index);
                }
            }

            pt = strtok(NULL ,";");
            count++;
        }
        count = 0;
    }

    if (NULL == arq)
        printf("Arquivo instrucoes.txt não encontrado.\n");
    fclose(arq);
}

void imprimirRegistradores(){
    printf("                          Registradores                          \n");
    printf("----------+----------+----------+----------+-------------------+\n");
    printf(" PC: %x    | MBR: %x   | MAR: %x   | IMM: %x   | IR %x \n", pc, mbr, mar, imm, ir);
    printf("----------+----------+----------+----------+--------+----------+\n");
    printf(" E: %x     | L: %x     | G: %x     | RO0: %x   | RO1: %x |  RO2: %x\n", e, l, g, ro0, ro1, ro2);
    printf("----------+----------+----------+------------------------------+\n");
    printf(" REG01: %x | REG02: %x | REG03: %x | REG04: %x  \n", reg[0], reg[1], reg[2], reg[3]);
    printf("----------+----------+----------+------------------------------+\n");
    printf(" REG05: %x | REG06: %x | REG07: %x | REG08: %x \n", reg[4], reg[5], reg[6], reg[7]);
    printf("----------+----------+----------+------------------------------+\n");
    printf(" REG09: %x | REG10: %x | REG11: %x | REG12: %x  \n", reg[8], reg[9], reg[10], reg[11]);
    printf("----------+----------+----------+------------------------------+\n");
    printf(" REG13: %x | REG14: %x | REG15: %x | REG16: %x \n", reg[12], reg[13], reg[14], reg[15]);
    printf("----------+----------+----------+------------------------------+\n");
}

void imprimirMemoria(){
    printf("\n                           Memoria                           ");
    for(unsigned int i = 0; i < 154; i++) {
        if (i%11 == 0)//adiciona quebra de linha a cada 11 elementos
            printf("\n----------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+\n|");
        printf(" %02x = %02x |", i, memoria[i]);
    };
}

/**
 * Imprime no terminal uma logo feita em um arquivo 'ifg-logo.txt'
 */
void imprimirLogo() {
    FILE *arquivo;
    char linha[90];

    if ((arquivo = fopen("ifg-logo.txt", "r")) != NULL) {
        while (fgets(linha, 90, arquivo) != NULL) {
            printf("%s", linha);
        }
        printf("\n\n\n");
        fclose(arquivo);
    }
}

int main(){
    imprimirLogo();
    lerArquivo();
    imprimirRegistradores();
    imprimirMemoria();
    return 0;
}

