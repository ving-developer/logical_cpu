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
unsigned char ir = 99; //5 bits
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

/**
 * Preenche a memoria com o padrão de mensagem que adiciona o opcode da instrucao nos primeiros 5 bits e completa com zero os demais
 *
 * @param opcode inteiro sem sinal representando os bits do opcode da instrucao
 * @param posicao posicao da memoria que deve ser salvo a palavra na memoria
 */
void preencherMemoriaPrimeiroFormato(unsigned int opcode, unsigned int posicao){
    opcode = opcode << 27;//desloca o opcode para os 5 bits mais siginificativos que representam o opcode
    guardarValorParaMemoria(posicao, opcode);//guarda na memoria a palavra 32 bits
}

/**
 * preenche a memoria com o padrao de mensagem que adiciona o opcode da instrucao nos primeiros 5 bits e um registrador nos proximos 4 bits
 * completando com 0 a esquerda o restante
 *
 * @param opcode inteiro sem sinal representando os bits do opcode da instrucao
 * @param rX registrador X para ser utilizado na instrucao
 * @param posicao posicao da memoria que deve ser salvo a palavra na memoria
 */
void preencherMemoriaSegundoFormato(unsigned int opcode, unsigned int rX, unsigned int posicao){
    unsigned int valor = opcode << 27;//desloca o opcode para os 5 bits mais siginificativos que representam o opcode
    valor = (rX << 23) | valor;//desloca o valor de rX para os 9 bits mais significativos que representarão na variavel valor os 5 bits do opcode + 4 bits do rX
    guardarValorParaMemoria(posicao, valor);//guarda na memoria a palavra 32 bits
}

/**
 * preenche a memoria com o padrao de mensagem que adiciona o opcode da instrucao nos primeiros 5 bits, um registrador nos proximos 4 bits
 * e outro registrador nos proximos 4 bits
 *
 * @param opcode inteiro sem sinal representando os bits do opcode da instrucao
 * @param rX registrador X para ser anexado na palavra
 * @param rY registrador Y para ser anexado na palavra
 * @param posicao posicao da memoria que deve ser salvo a palavra na memoria
 */
void preencherMemoriaTerceiroFormato(unsigned int opcode, unsigned int rX, unsigned int rY, unsigned int posicao){
    unsigned int valor = opcode << 27;//desloca o opcode para os 5 bits mais siginificativos que representam o opcode
    valor = (rX << 23) | valor;//desloca o valor de rX para os 9 bits mais significativos que representarão na variavel valor os 5 bits do opcode + 4 bits do rX
    valor = (rY << 19) | valor;//desloca o valor de rY para os 13 bits mais significativos que representarão na variavel valor os 5 bits do opcode + 4 bits do rX + 4 bits do rY
    guardarValorParaMemoria(posicao, valor);//guarda na memoria a palavra 32 bits
}

/**
 * preenche a memoria com o padrao de mensagem que adiciona o opcode da instrucao nos primeiros 5 bits, primeiro registrador nos proximos 4 bits,
 * segundo registrador nos proximos 4 bits e uma posicao da memoria nos proximos 19 bits
 *
 * @param opcode inteiro sem sinal representando os bits do opcode da instrucao
 * @param rX registrador X para ser anexado na palavra
 * @param rY registrador Y para ser anexado na palavra
 * @param posMem posicao da memoria para ser anexado na palavra
 * @param posicao posicao da memoria que deve ser salvo a palavra na memoria
 */
void preencherMemoriaQuartoFormato(unsigned int opcode, unsigned int rX, unsigned int rY, unsigned int posMem, unsigned int posicao){
    unsigned int valor = opcode << 27;//desloca o opcode para os 5 bits mais siginificativos que representam o opcode
    valor = (rX << 23) | valor;//desloca o valor de rX para os 9 bits mais significativos que representarão na variavel valor os 5 bits do opcode + 4 bits do rX
    valor = (rY << 19) | valor;//desloca o valor de rY para os 13 bits mais significativos que representarão na variavel valor os 5 bits do opcode + 4 bits do rX + 4 bits do rY
    valor = posMem | valor;//desloca o valor de rY para os 13 bits mais significativos que representarão na variavel valor os 5 bits do opcode + 4 bits do rX e deixa os outros 14 bits para representar a posição da memoria na palavra
    guardarValorParaMemoria(posicao, valor);//guarda na memoria a palavra 32 bits
}

/**
 * preenche a memoria com o padrao de mensagem que adiciona o opcode da instrucao nos primeiros 5 bits, primeiro registrador nos proximos 4 bits,
 * segundo registrador nos proximos 4 bits e terceiro registrador nos proximos 4 bits, completando o restante com zeros
 * @param opcode inteiro sem sinal representando os bits do opcode da instrucao
 * @param rX registrador X para ser anexado na palavra
 * @param rY registrador Y para ser anexado na palavra
 * @param rZ registrador Z para ser anexado na palavra
 * @param posicao posicao da memoria que deve ser salvo a palavra na memoria
 */
void preencherMemoriaQuintoFormato(unsigned int opcode, unsigned int rX, unsigned int rY,  unsigned int rZ, unsigned int posicao){
    unsigned int valor = opcode << 27;//desloca o opcode para os 5 bits mais siginificativos que representam o opcode
    valor = (rX << 23) | valor;//desloca o valor de rX para os 9 bits mais significativos que representarão na variavel valor os 5 bits do opcode + 4 bits do rX
    valor = (rY << 19) | valor;//desloca o valor de rY para os 13 bits mais significativos que representarão na variavel valor os 5 bits do opcode + 4 bits do rX + 4 bits do rY
    valor = (rZ << 15) | valor;//desloca o valor de rY para os 17 bits mais significativos que representarão na variavel valor os 5 bits do opcode + 4 bits do rX + 4 bits do rX + 4 bits do rY + 4 bits rZ
    guardarValorParaMemoria(posicao, valor);//guarda na memoria a palavra 32 bits
}

/**
 * preenche a memoria com o padrao de mensagem que adiciona o opcode da instrucao nos primeiros 5 bits, primeiro registrador nos proximos 4 bits,
 * e a posicao de memoria nos ultimos 23 bits
 * @param opcode inteiro sem sinal representando os bits do opcode da instrucao
 * @param rX registrador X para ser anexado na palavra
 * @param posMem posicao da memoria para ser anexado na palavra
 * @param posicao posicao da memoria que deve ser salvo a palavra na memoria
 */
void preencherMemoriaSextoFormato(unsigned int opcode, unsigned int rX, unsigned int posMem, unsigned int posicao){
    unsigned int valor = opcode << 27;//desloca o opcode para os 5 bits mais siginificativos que representam o opcode
    valor = (rX << 23) | valor;//desloca o valor de rX para os 9 bits mais significativos que representarão na variavel valor os 5 bits do opcode + 4 bits do rX
    valor = posMem | valor;//desloca o valor de rX para os 9 bits mais significativos que representarão na variavel valor os 5 bits do opcode + 4 bits do rX e deixa os outros 23 bits para registrar a posição da memoria na palavra
    guardarValorParaMemoria(posicao, valor);//guarda na memoria a palavra 32 bits
}

/**
 * preenche a memoria com o padrao de mensagem que adiciona o opcode da instrucao nos primeiros 5 bits, preenche 4 bits com zero
 * e guarda a posicao de memoria nos ultimos 23 bits
 * @param opcode inteiro sem sinal representando os bits do opcode da instrucao
 * @param posMem posicao da memoria para ser anexado na palavra
 * @param posicao posicao da memoria que deve ser salvo a palavra na memoria
 */
void preencherMemoriaSetimoFormato(unsigned int opcode, unsigned int posMem, unsigned int posicao){
    unsigned int valor = opcode << 27;//desloca o opcode para os 5 bits mais siginificativos que representam o opcode
    valor = (0 << 23) | valor;//desloca 23 bits a direita preenchendo com 0 os 4 bits subsequentes
    valor = posMem | valor;//desloca o valor de rX para os 9 bits mais significativos que representarão na variavel valor os 5 bits do opcode + 4 bits do rX e deixa os outros 23 bits para registrar a posição da memoria na palavra
    guardarValorParaMemoria(posicao, valor);//guarda na memoria a palavra 32 bits
}

/**
 *  processa o minemonico de instrucao criando uma palavra de 32 bits para representa-la
 *  e guarda em 4 posicoes da memoria a palavra de 32 bits
 *
 * @param mem posicao da memoria em que a palavra de instrucao deve ser salva
 */
void processarInstrucao(int mem){
    unsigned char *ponteiro, instrucao[8];
    unsigned int count = 0,reg0, reg1, imediatoMem;

    ponteiro = strtok(pt," ,");//divide a palavra de instrucao em pedacos divididos por , ou espaco

//    for (ponteiro = strtok(pt, " ,"); ponteiro != NULL; ponteiro = strtok(NULL, " ,")) { }
    while(ponteiro){
        if(count == 0){
            strcpy(instrucao, ponteiro);
        }
        if (count== 1){
            reg0 = (int)strtol(ponteiro, NULL, 16);
        }
        if(count == 2){
            reg1 = (int)strtol(ponteiro,NULL,16);
        }
        if(count == 3){
            imediatoMem = (int)strtol(ponteiro,NULL,16);
        }
        ponteiro = strtok(NULL," ,r");
        count++;
    }

    if(strstr(instrucao, "hlt") != NULL) {
        preencherMemoriaPrimeiroFormato(0, mem);
    } if(strstr(instrucao, "nop") != NULL) {
        preencherMemoriaPrimeiroFormato(1, mem);
    } else if(strcmp(instrucao, "not") == 0) {
        preencherMemoriaSegundoFormato(2, reg0, mem);
    } else if(strcmp(instrucao, "movr") == 0){
        preencherMemoriaTerceiroFormato(3, reg0, reg1, mem);
    } else if(strcmp(instrucao, "cmp") == 0){
        preencherMemoriaTerceiroFormato(4, reg0, reg1, mem);
    } else if(strcmp(instrucao, "ldbo") == 0){
        preencherMemoriaQuartoFormato(5, reg0, reg1, imediatoMem, mem);
    } else if(strcmp(instrucao, "stbo") == 0){
        preencherMemoriaQuartoFormato(6, reg0, reg1, imediatoMem, mem);
    } else if(strcmp(instrucao, "add") == 0){
        preencherMemoriaQuintoFormato(7, reg0, reg1, imediatoMem, mem);
    } else if(strcmp(instrucao, "sub") == 0){
        preencherMemoriaQuintoFormato(8, reg0, reg1, imediatoMem, mem);
    } else if(strcmp(instrucao, "mul") == 0){
        preencherMemoriaQuintoFormato(9, reg0, reg1, imediatoMem, mem);
    } else if(strcmp(instrucao, "div") == 0){
        preencherMemoriaQuintoFormato(10, reg0, reg1, imediatoMem, mem);
    } else if(strcmp(instrucao, "and") == 0){
        preencherMemoriaQuintoFormato(11, reg0, reg1, imediatoMem, mem);
    } else if(strcmp(instrucao, "or") == 0){
        preencherMemoriaQuintoFormato(12, reg0, reg1, imediatoMem, mem);
    } else if(strcmp(instrucao, "xor") == 0){
        preencherMemoriaQuintoFormato(13, reg0, reg1, imediatoMem, mem);
    } else if(strcmp(instrucao, "ld") == 0){
        preencherMemoriaSextoFormato(14, reg0, reg1, mem);
    } else if(strcmp(instrucao, "st") == 0){
        preencherMemoriaSextoFormato(15, reg0, reg1, mem);
    } else if(strcmp(instrucao, "movil") == 0){
        preencherMemoriaSextoFormato(16, reg0, reg1, mem);
    } else if(strcmp(instrucao, "movih") == 0){
        preencherMemoriaSextoFormato(17, reg0, reg1, mem);
    } else if(strcmp(instrucao, "addi") == 0){
        preencherMemoriaSextoFormato(18, reg0, reg1, mem);
    } else if(strcmp(instrucao, "subi") == 0){
        preencherMemoriaSextoFormato(19, reg0, reg1, mem);
    } else if(strcmp(instrucao, "muli") == 0){
        preencherMemoriaSextoFormato(20, reg0, reg1, mem);
    } else if(strcmp(instrucao, "divi") == 0){
        preencherMemoriaSextoFormato(21, reg0, reg1, mem);
    } else if(strcmp(instrucao, "lsh") == 0){
        preencherMemoriaSextoFormato(22, reg0, reg1, mem);
    } else if(strcmp(instrucao, "rsh") == 0){
        preencherMemoriaSextoFormato(23, reg0, reg1, mem);
    } else if(strcmp(instrucao, "je") == 0){
        preencherMemoriaSetimoFormato(24, reg0, mem);
    } else if(strcmp(instrucao, "jne") == 0){
        preencherMemoriaSetimoFormato(25, reg0, mem);
    } else if(strcmp(instrucao, "jl") == 0){
        preencherMemoriaSetimoFormato(26, reg0, mem);
    } else if(strcmp(instrucao, "jle") == 0){
        preencherMemoriaSetimoFormato(27, reg0, mem);
    } else if(strcmp(instrucao, "jg") == 0){
        preencherMemoriaSetimoFormato(28, reg0, mem);
    } else if(strcmp(instrucao, "jge") == 0){
        preencherMemoriaSetimoFormato(29, reg0, mem);
    } else if(strcmp(instrucao, "jmp") == 0){
        preencherMemoriaSetimoFormato(30, reg0, mem);
    }
}

/**
 * limpa a tela do terminal e move cursor para o inicio
 */
void limparTela() {
    printf("\033[2J\033[H");
}

/**
 *  Pergunta o usuario se deseja continuar a execucao do programa, caso usuario insira Enter continua a execucao
 *  caso contrario, interrompe a execucao do programa
 */
void solicitaContinuar(){
    printf("\n\nDigite Enter para continuar (outra tecla para sair)...\n");
    int c = getchar();
    limparTela();
    if (c != '\n') { // Se não for outro Enter, cancela a execução
        printf("Programa encerrado por interrupção do usuario.\n");
        exit(0);
    }
}

/**
 * Imprime todos os registradores no terminal
 */
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

/**
 * Imprime todas as posicoes de memoria no terminal
 */
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

    solicitaContinuar();
}

/**
 * Busca pelo arquivo instrucoes.txt no mesmo diretorio do executavel e processa o arquivo
 * inserindo a instrução na memoria caso seja uma linha de instrução ou guardando o valor na memoria
 * caso seja uma palavra de dado
 */
void processarArquivo(){
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
                    processarInstrucao(index);
                }
            }

            pt = strtok(NULL ,";");
            count++;
        }
        count = 0;
    }

    if (NULL == arq){
        fclose(arq);
        printf("Arquivo instrucoes.txt não encontrado.\n");
        exit(0);
    } else {
        printf("Arquivo instrucoes.txt PROCESSADO!\n");
        imprimirMemoria();
        solicitaContinuar();
    }
}

void ciclo() {
    mar = pc;
    mbr = memoria[mar++] << 8;
    mbr = (mbr | memoria[mar++]) << 8;
    mbr = (mbr | memoria[mar++]) << 8;
    mbr = mbr | memoria[mar++];
    //opcode com os 5 bits mais significativos
    ir = (mbr >> 27);

    if (ir == 0 || ir == 1) {
        if (ir == 0) { // hlt
            printf("Programa chegou ao fim (hlt).\n\n\n\n");
            exit(0);
            //CPU n�o faz nada, hlt, finalizar programa
        } else if (ir == 1){  //nop
            //apenas incrementar o PC
            pc += 4;
        }
    } else if (ir >= 2 && ir <= 4){
        ro0 = ((mbr & 0x07800000) >> 23);
        ro1 = (mbr &  0x00780000) >> 19;
        if (ir == 2) { //not
            ro0 = ((mbr & 0x07800000) >> 23);
            reg[ro0] = !reg[ro0];
            pc += 4;
        } else if (ir == 3) { //movr
            reg[ro0] = reg[ro1];
            pc += 4;
        } else if (ir == 4) { //cmp
            if (reg[ro0] == reg[ro1]){
                e = 0x01;
                pc += 4;
            } else {
                e = 0x00;
                pc += 4;
            }
            if (reg[ro0] < reg[ro1]){
                l = 0x01;
                pc += 4;
            } else {
                l = 0x00;
                pc += 4;
            }
            if (reg[ro0] > reg[ro1]){
                g = 0x01;
                pc += 4;
            } else {
                g = 0x00;
                pc += 4;
            }
        }
    }

    //operacoes logicas e aritmeticas
    else if ( ir >= 7 & ir <= 13) {
        ro0 = (mbr &  0x07800000) >> 23;
        ro1 = (mbr &  0x00780000) >> 19;
        ro2 = (mbr &  0x00078000) >> 15;
        if (ir == 7){ //add
            reg[ro0] = reg[ro1] + reg[ro2];
            pc+=4;
        }
        else if (ir == 8){ //sub
            reg[ro0] = reg[ro1] - reg[ro2];
            pc+=4;
        }
        else if (ir == 9){ //mul
            reg[ro0] = reg[ro1] * reg[ro2];
            pc+=4;
        }
        else if (ir == 10){ //div
            reg[ro0] = reg[ro1] / reg[ro2];
            pc+=4;
        }
        else if (ir == 11){ //and
            reg[ro0] = reg[ro1] & reg[ro2];
            pc+=4;
        }
        else if (ir == 12){ //or
            reg[ro0] = reg[ro1] | reg[ro2];
            pc+=4;
        }
        else if (ir == 13){ //xor
            reg[ro0] = reg[ro1] ^ reg[ro2];
            pc+=4;
        }
    }

    else if (ir == 14 || ir == 15) {
        ro0 = (mbr &  0x07800000) >> 23;
        //ro1 = (mbr &  0x780000) >> 19;
        mar = (mbr & 0x007fffff);
        if (ir == 14) { //ld
            mbr = memoria[mar++];
            mbr = (mbr << 8) | memoria[mar++];
            mbr = (mbr << 8) | memoria[mar++];
            mbr = (mbr << 8) | memoria[mar++];
            reg[ro0] = mbr;
            pc += 4;
        } else if (ir == 15) { //st
            mbr = reg[ro0];
            memoria[mar++] = (mbr >> 24);
            memoria[mar++] = (mbr >> 16);
            memoria[mar++] = (mbr >> 8);
            memoria[mar++] = mbr;
            pc += 4;
        }
    }
    else if (ir >= 18 && ir <= 23) {
        ro0 = (mbr &  0x07800000) >> 23;
        imm = (mbr & 0x007fffff);
        if (ir == 18) { //addi
            reg[ro0] = (reg[ro0] + imm);
            pc += 4;
        } else if (ir == 19) { //subi
            reg[ro0] = (reg[ro0] - imm);
            pc += 4;
        } else if (ir == 20) { //muli
            reg[ro0] = (reg[ro0] * imm);
            pc += 4;
        } else if (ir == 21) { //divi
            reg[ro0] = (reg[ro0] / imm);
            pc += 4;
        } else if (ir == 22) { //lsh
            reg[ro0] = (reg[ro0] << imm);
            pc += 4;
        } else if (ir == 23) { //rsh
            reg[ro0] = (reg[ro0] >> imm);
            pc += 4;
        }
    }
    else if (ir >= 24 && ir <= 30){
        mar = (mbr & 0x007fffff);

        if (ir == 24) { //je
            if (e == 0x01) {
                pc = mar;
            }
        } else if (ir == 25) { //jne
            if (e == 0x00) {
                pc = mar;
            }
        } else if (ir == 26) { //jl
            if (l == 0x01) {
                pc = mar;
            }
        } else if (ir == 27) { //jle
            if (e == 0x01 || l == 0x01) {
                pc = mar;
            }
        } else if (ir == 28) { //jg
            if (g == 0x01) {
                pc = mar;
            }
        } else if (ir == 29) { //jge
            if (e == 0x01 || g == 0x01) {
                pc = mar;
            }
        } else if (ir == 30) { //jmp
            pc = mar;
        }
    }
}

int main(){
    imprimirLogo();
    processarArquivo();
    while(1){
        ciclo();
        imprimirRegistradores();
        imprimirMemoria();
        solicitaContinuar();
    }
}

