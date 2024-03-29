#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned int mbr, mar = 0, imm, pc = 0, reg[16];
unsigned char ir, ro0, ro1, ro2, e, l, g, memoria[154];

// Funções Auxiliares
void logical_not(unsigned int *reg) {
    *reg = ~(*reg); // Inverte todos os bits do registrador
}

void move_register(unsigned int *dest, unsigned int src) {
    *dest = src; // Move o valor do registrador de origem para o destino
}

void compare_registers(unsigned int regX, unsigned int regY) {
    // Comparação dos registradores e preenchimento dos registradores internos E, L e G
    if (regX == regY) {
        e = 1;
    } else {
        e = 0;
    }

    if (regX < regY) {
        l = 1;
    } else {
        l = 0;
    }

    if (regX > regY) {
        g = 1;
    } else {
        g = 0;
    }
}

void load_via_base_offset(unsigned int regX, unsigned int regY, unsigned int Z) {
    // Load via base + offset: regX = *(M[Z] + regY)
    mar = memoria[Z] + regY;
    mbr = memoria[mar];
    *regX = mbr;
}

void store_via_base_offset(unsigned int regX, unsigned int regY, unsigned int Z) {
    // Store via base + offset: *(M[Z] + regY) = regX
    mar = memoria[Z] + regY;
    memoria[mar] = *regX;
}

// Instruções aritméticas e lógicas
void add_registers(unsigned int regX, unsigned int regY, unsigned int regZ) {
    // ADD REGISTER instruction: regX = regY + regZ
    regX = regY + regZ;
}

void sub_registers(unsigned int regX, unsigned int regY, unsigned int regZ) {
    // SUBTRACT REGISTER instruction: regX = regY - regZ
    regX = regY - regZ;
}

void mul_registers(unsigned int regX, unsigned int regY, unsigned int regZ) {
    // MULTIPLY REGISTER instruction: regX = regY * regZ
    regX = regY * regZ;
}

void div_registers(unsigned int regX, unsigned int regY, unsigned int regZ) {
    // DIVIDE REGISTER instruction: regX = regY / regZ
    regX = regY / regZ;
}

void and_registers(unsigned int regX, unsigned int regY, unsigned int regZ) {
    // LOGICAL-AND ON REGISTER instruction: regX = regY & regZ
    regX = regY & regZ;
}

void or_registers(unsigned int regX, unsigned int regY, unsigned int regZ) {
    // LOGICAL-OR ON REGISTER instruction: regX = regY | regZ
    regX = regY | regZ;
}

void xor_registers(unsigned int regX, unsigned int regY, unsigned int regZ) {
    // LOGICAL-XOR ON REGISTER instruction: regX = regY ^ regZ
    regX = regY ^ regZ;
}

void left_shift(unsigned int *regX, unsigned int imm) {
    // LEFT SHIFT instruction: desloca a palavra no registrador X em IMM bits à esquerda
    *regX = *regX << imm;
}

void right_shift(unsigned int *regX, unsigned int imm) {
    // RIGHT SHIFT instruction: desloca a palavra no registrador X em IMM bits à direita
    *regX = *regX >> imm;
}

void load_instruction(unsigned int mbr) {
    unsigned int opcode = (mbr >> 27) & 0x1F; // Extrai o opcode

    if (opcode == 0b00101) {
        // LOAD VIA BASE+OFFSET instruction
        unsigned int ro0 = (mbr >> 24) & 0x0F; // Extrai o registrador ro0
        unsigned int ro1 = (mbr >> 20) & 0x0F; // Extrai o registrador ro1
        unsigned int ro2 = (mbr >> 16) & 0x0F; // Extrai o registrador ro2
        unsigned int offset = mbr & 0xFFFF; // Extrai o offset

        // Carrega para o registrador ro0 uma palavra da memória que se inicia no endereço ro1 + ro2 + offset
        reg[ro0] = *((unsigned int*)&memoria[reg[ro1] + reg[ro2] + offset]);
    } else if (opcode == 0b01110) {
        // LOAD instruction
        unsigned int ro0 = (mbr >> 24) & 0x0F; // Extrai o registrador ro0
        unsigned int address = mbr & 0x00FFFFFF; // Extrai o endereço de memória

        // Carrega para o registrador ro0 uma palavra da memória de 32 bits que se inicia no endereço address
        reg[ro0] = *((unsigned int*)&memoria[address]);
    }
}

void store_instruction(unsigned int mbr) {
    unsigned int opcode = (mbr >> 27) & 0x1F; // Extrai o opcode

    if (opcode == 0b00110) {
        // STORE VIA BASE+OFFSET instruction
        unsigned int ro0 = (mbr >> 24) & 0x0F; // Extrai o registrador ro0
        unsigned int ro1 = (mbr >> 20) & 0x0F; // Extrai o registrador ro1
        unsigned int ro2 = (mbr >> 16) & 0x0F; // Extrai o registrador ro2
        unsigned int offset = mbr & 0xFFFF; // Extrai o offset

        // Armazena uma palavra de 32 bits que começa a partir do endereço ro1 + ro2 + offset o conteúdo do registrador ro0
        *((unsigned int*)&memoria[reg[ro1] + reg[ro2] + offset]) = reg[ro0];
    } else if (opcode == 0b01111) {
        // STORE instruction
        unsigned int ro0 = (mbr >> 24) & 0x0F; // Extrai o registrador ro0
        unsigned int address = mbr & 0x00FFFFFF; // Extrai o endereço de memória

        // Armazena uma palavra de 32 bits que começa a partir do endereço de memória address o conteúdo do registrador ro0
        *((unsigned int*)&memoria[address]) = reg[ro0];
    }
}

void move_immediate_lower(unsigned int mbr) {
    // MOVE IMMEDIATE TO THE LOWER HALF OF THE REGISTER instruction
    unsigned int ro0 = (mbr >> 24) & 0x0F; // Extrai o registrador ro0
    unsigned int immediate = mbr & 0x0000FFFF; // Extrai os bits menos significativos do imediato

    // Zera o registrador ro0 e move os dezesseis bits menos significativos do imediato para a parte inferior do registrador ro0
    reg[ro0] &= 0xFFFF0000;
    reg[ro0] |= immediate;
}

void move_immediate_upper(unsigned int mbr) {
    // MOVE IMMEDIATE TO THE HIGHER HALF OF THE REGISTER instruction
    unsigned int ro0 = (mbr >> 24) & 0x0F; // Extrai o registrador ro0
    unsigned int immediate = mbr & 0x0000FFFF; // Extrai os bits menos significativos do imediato

    // Move os dezesseis bits menos significativos do imediato para a parte superior do registrador ro0, mantendo os bits menos significativos do registrador ro0 intactos
    reg[ro0] &= 0x0000FFFF;
    reg[ro0] |= (immediate << 16);
}

void add_immediate(unsigned int mbr) {
    // ADD IMMEDIATE instruction
    unsigned int ro0 = (mbr >> 24) & 0x0F; // Extrai o registrador ro0
    unsigned int immediate = mbr & 0x00FFFFFF; // Extrai o imediato

    // Adiciona o imediato ao valor do registrador ro0
    reg[ro0] += immediate;
}

void multiply_immediate(unsigned int mbr) {
    // MULTIPLY IMMEDIATE instruction
    unsigned int ro0 = (mbr >> 24) & 0x0F; // Extrai o registrador ro0
    unsigned int immediate = mbr & 0x00FFFFFF; // Extrai o imediato

    // Multiplica o valor do registrador ro0 pelo imediato
    reg[ro0] *= immediate;
}

void divide_immediate(unsigned int mbr) {
    // DIVIDE IMMEDIATE instruction
    unsigned int ro0 = (mbr >> 24) & 0x0F; // Extrai o registrador ro0
    unsigned int immediate = mbr & 0x00FFFFFF; // Extrai o imediato

    // Divide o valor do registrador ro0 pelo imediato
    reg[ro0] /= immediate;
}

void move_immediate_lower(unsigned int mbr) {
    // MOVE IMMEDIATE TO THE LOWER HALF OF THE REGISTER instruction
    unsigned int ro0 = (mbr >> 24) & 0x0F; // Extrai o registrador ro0
    unsigned int immediate_lower = mbr & 0x0000FFFF; // Extrai os bits menos significativos do imediato

    // Move os bits menos significativos do imediato para a parte inferior do registrador ro0
    reg[ro0] &= 0xFFFF0000; // Limpa os bits inferiores
    reg[ro0] |= immediate_lower; // Move os bits do imediato para a parte inferior
}

void move_immediate_upper(unsigned int mbr) {
    // MOVE IMMEDIATE TO THE HIGHER HALF OF THE REGISTER instruction
    unsigned int ro0 = (mbr >> 24) & 0x0F; // Extrai o registrador ro0
    unsigned int immediate_upper = (mbr & 0x0000FFFF) << 16; // Extrai os bits menos significativos do imediato e desloca para a parte superior

    // Move os bits do imediato para a parte superior do registrador ro0, mantendo os bits inferiores intactos
    reg[ro0] &= 0x0000FFFF; // Limpa os bits superiores
    reg[ro0] |= immediate_upper; // Move os bits do imediato para a parte superior
}

void add_immediate(unsigned int mbr) {
    // ADD IMMEDIATE instruction
    unsigned int ro0 = (mbr >> 24) & 0x0F; // Extrai o registrador ro0
    unsigned int immediate = mbr & 0x00FFFFFF; // Extrai o imediato

    // Adiciona o imediato ao valor do registrador ro0
    reg[ro0] += immediate;
}

void multiply_immediate(unsigned int mbr) {
    // MULTIPLY IMMEDIATE instruction
    unsigned int ro0 = (mbr >> 24) & 0x0F; // Extrai o registrador ro0
    unsigned int immediate = mbr & 0x00FFFFFF; // Extrai o imediato

    // Multiplica o valor do registrador ro0 pelo imediato
    reg[ro0] *= immediate;
}

void lsh(unsigned int mbr) {
    // LEFT SHIFT instruction
    unsigned int ro0 = (mbr >> 24) & 0x0F; // Extrai o registrador ro0
    unsigned int shift_amount = mbr & 0x0000000F; // Extrai a quantidade de deslocamento

    // Desloca a palavra no registrador ro0 para a esquerda em shift_amount bits
    reg[ro0] <<= shift_amount;
}

void rsh(unsigned int mbr) {
    // RIGHT SHIFT instruction
    unsigned int ro0 = (mbr >> 24) & 0x0F; // Extrai o registrador ro0
    unsigned int shift_amount = mbr & 0x0000000F; // Extrai a quantidade de deslocamento

    // Desloca a palavra no registrador ro0 para a direita em shift_amount bits
    reg[ro0] >>= shift_amount;
}

void je(unsigned int mbr) {
    // JUMP IF EQUAL TO instruction
    unsigned int address = mbr & 0x00FFFFFF; // Extrai o endereço de memória

    // Muda o registrador PC para o endereço de memória se E = 1
    if (E) {
        PC = address;
    }
}

void jne(unsigned int mbr) {
    // JUMP IF NOT EQUAL TO instruction
    unsigned int address = mbr & 0x00FFFFFF; // Extrai o endereço de memória

    // Muda o registrador PC para o endereço de memória se E = 0
    if (!E) {
        PC = address;
    }
}

void jl(unsigned int mbr) {
    // JUMP IF LOWER THAN instruction
    unsigned int address = mbr & 0x00FFFFFF; // Extrai o endereço de memória

    // Muda o registrador PC para o endereço de memória se L = 1
    if (L) {
        PC = address;
    }
}

void jle(unsigned int mbr) {
    // JUMP IF LOWER THAN OR EQUAL TO instruction
    unsigned int address = mbr & 0x00FFFFFF; // Extrai o endereço de memória

    // Muda o registrador PC para o endereço de memória se E = 1 ou L = 1
    if (E || L) {
        PC = address;
    }
}

void jg(unsigned int mbr) {
    // JUMP IF GREATER THAN instruction
    unsigned int address = mbr & 0x00FFFFFF; // Extrai o endereço de memória

    // Muda o registrador PC para o endereço de memória se G = 1
    if (G) {
        PC = address;
    }
}

void jge(unsigned int mbr) {
    // JUMP IF GREATER THAN OR EQUAL TO instruction
    unsigned int address = mbr & 0x00FFFFFF; // Extrai o endereço de memória

    // Muda o registrador PC para o endereço de memória se E = 1 ou G = 1
    if (E || G) {
        PC = address;
    }
}

void jmp(unsigned int mbr) {
    // JUMP instruction
    unsigned int address = mbr & 0x00FFFFFF; // Extrai o endereço de memória

    // Muda o registrador PC para o endereço de memória
    PC = address;
}

void load_instruction(unsigned int mbr) {
    // LOAD instruction
    unsigned int ro0 = (mbr >> 24) & 0x0F; // Extrai o registrador ro0
    unsigned int address = mbr & 0x00FFFFFF; // Extrai o endereço de memória

    // Carrega para o registrador ro0 uma palavra de 32 bits da memória a partir do endereço especificado
    reg[ro0] = memoria[address];
}

void store_instruction(unsigned int mbr) {
    // STORE instruction
    unsigned int ro0 = (mbr >> 24) & 0x0F; // Extrai o registrador ro0
    unsigned int address = mbr & 0x00FFFFFF; // Extrai o endereço de memória

    // Armazena uma palavra de 32 bits do registrador ro0 na memória a partir do endereço especificado
    memoria[address] = reg[ro0];
}

void ciclo() {
    mar = pc;
    mbr = memoria[mar++];
    mbr = (mbr << 8) | memoria[mar++];
    mbr = (mbr << 8) | memoria[mar++];
    mbr = (mbr << 8) | memoria[mar++];

    ir = (mbr >> 27); // Obtém os 5 bits mais significativos para identificar a instrução

    if (ir == 0b00000) {
        // HALT instruction
        // Não faz nada, apenas termina a execução do programa
        return;
    } else if (ir == 0b00001) {
        // NO OPERATION instruction
        // Incrementa o PC
        pc += 4;
    } else if (ir == 0b00010) {
        // LOGICAL-NOT ON REGISTER instruction
        ro0 = ((mbr & 0xf00000) >> 20); // Obtém o registrador alvo
        logical_not(&reg[ro0]); // Chama a função para inverter o valor do registrador
        pc += 4; // Incrementa o PC
    } else if (ir == 0b00011) {
        // MOVE REGISTER instruction
        ro0 = ((mbr & 0xf00000) >> 20); // Registrador de destino
        ro1 = ((mbr & 0x0f0000) >> 16); // Registrador de origem
        move_register(&reg[ro0], reg[ro1]); // Move o valor do registrador de origem para o destino
        pc += 4; // Incrementa o PC
    } else if (ir == 0b00100) {
        // COMPARE REGISTER instruction
        ro0 = ((mbr & 0xf00000) >> 20); // Registrador X
        ro1 = ((mbr & 0x0f0000) >> 16); // Registrador Y
        compare_registers(reg[ro0], reg[ro1]); // Compara os registradores e preenche E, L e G
        pc += 4; // Incrementa o PC
    } else if (ir == 0b00101) {
        // LOAD VIA BASE+OFFSET instruction
        ro0 = ((mbr & 0xf00000) >> 20); // Registrador de destino
        ro1 = ((mbr & 0x0f0000) >> 16); // Registrador base
        imm = (mbr & 0x0000ffff); // Offset

        load_via_base_offset(reg[ro0], reg[ro1], imm); // Carrega o valor via base + offset
        pc += 4; // Incrementa o PC
    } else if (ir == 0b00110) {
        // STORE VIA BASE+OFFSET instruction
        ro0 = ((mbr & 0xf00000) >> 20); // Registrador fonte
        ro1 = ((mbr & 0x0f0000) >> 16); // Registrador base
        imm = (mbr & 0x0000ffff); // Offset

        store_via_base_offset(reg[ro0], reg[ro1], imm); // Armazena o valor via base + offset
        pc += 4; // Incrementa o PC
    } else if (ir == 0b00111) {
        // ADD REGISTER instruction
        ro0 = ((mbr & 0xf00000) >> 20); // Registrador de destino
        ro1 = ((mbr & 0x0f0000) >> 16); // Registrador fonte 1
        ro2 = ((mbr & 0x0000f00) >> 8); // Registrador fonte 2

        add_registers(reg[ro0], reg[ro1], reg[ro2]); // Realiza a adição
        pc += 4; // Incrementa o PC
    } else if (ir == 0b01000) {
        // SUBTRACT REGISTER instruction
        ro0 = ((mbr & 0xf00000) >> 20); // Registrador de destino
        ro1 = ((mbr & 0x0f0000) >> 16); // Registrador fonte 1
        ro2 = ((mbr & 0x0000f00) >> 8); // Registrador fonte 2

        sub_registers(reg[ro0], reg[ro1], reg[ro2]); // Realiza a subtração
        pc += 4; // Incrementa o PC
    } else if (ir == 0b01001) {
        // MULTIPLY REGISTER instruction
        ro0 = ((mbr & 0xf00000) >> 20); // Registrador de destino
        ro1 = ((mbr & 0x0f0000) >> 16); // Registrador fonte 1
        ro2 = ((mbr & 0x0000f00) >> 8); // Registrador fonte 2

        mul_registers(reg[ro0], reg[ro1], reg[ro2]); // Realiza a multiplicação
        pc += 4; // Incrementa o PC
    } else if (ir == 0b01010) {
        // DIVIDE REGISTER instruction
        ro0 = ((mbr & 0xf00000) >> 20); // Registrador de destino
        ro1 = ((mbr & 0x0f0000) >> 16); // Registrador fonte 1
        ro2 = ((mbr & 0x0000f00) >> 8); // Registrador fonte 2

        div_registers(reg[ro0], reg[ro1], reg[ro2]); // Realiza a divisão
        pc += 4; // Incrementa o PC
    } else if (ir == 0b01011) {
        // LOGICAL-AND ON REGISTER instruction
        ro0 = ((mbr & 0xf00000) >> 20); // Registrador de destino
        ro1 = ((mbr & 0x0f0000) >> 16); // Registrador fonte 1
        ro2 = ((mbr & 0x0000f00) >> 8); // Registrador fonte 2

        and_registers(reg[ro0], reg[ro1], reg[ro2]); // Realiza a operação lógica AND
        pc += 4; // Incrementa o PC
    } else if (ir == 0b01100) {
        // LOGICAL-OR ON REGISTER instruction
        ro0 = ((mbr & 0xf00000) >> 20); // Registrador de destino
        ro1 = ((mbr & 0x0f0000) >> 16); // Registrador fonte 1
        ro2 = ((mbr & 0x0000f00) >> 8); // Registrador fonte 2

        or_registers(reg[ro0], reg[ro1], reg[ro2]); // Realiza a operação lógica OR
        pc += 4; // Incrementa o PC
    } else if (ir == 0b01101) {
        // LOGICAL-XOR ON REGISTER instruction
        ro0 = ((mbr & 0xf00000) >> 20); // Registrador de destino
        ro1 = ((mbr & 0x0f0000) >> 16); // Registrador fonte 1
        ro2 = ((mbr & 0x0000f00) >> 8); // Registrador fonte 2

        xor_registers(reg[ro0], reg[ro1], reg[ro2]); // Realiza a operação lógica XOR
        pc += 4; // Incrementa o PC
    } else if (ir == 0b01110) {
        // LOAD instruction
        ro0 = ((mbr & 0xf00000) >> 20); // Registrador de destino
        imm = (mbr& 0x000ffff); // Endereço de memória

        // Carrega para o registrador X uma palavra da memória de 32 bits que se inicia no endereço Y
        load_instruction(&reg[ro0], imm);

        pc += 4; // Incrementa o PC
    } else if (ir == 0b01111) {
        // STORE instruction
        ro0 = ((mbr & 0xf00000) >> 20); // Registrador fonte
        imm = (mbr & 0x000ffff); // Endereço de memória

        // Armazena uma palavra de 32 bits que começa a partir do endereço de memória Y o conteúdo do registrador X
        store_instruction(reg[ro0], imm);

        pc += 4; // Incrementa o PC
    } else if (ir == 0b10000) {
        // MOVE IMMEDIATE TO THE LOWER HALF OF THE REGISTER instruction
        ro0 = ((mbr & 0xf00000) >> 20); // Registrador de destino
        imm = (mbr & 0x0000ffff); // Valor imediato

        // Zera o registrador X e move os dezesseis bits menos significativos do imediato para a parte inferior do registrador X
        move_immediate_lower(&reg[ro0], imm);

        pc += 4; // Incrementa o PC
    } else if (ir == 0b10001) {
        // MOVE IMMEDIATE TO THE HIGHER HALF OF THE REGISTER instruction
        ro0 = ((mbr & 0xf00000) >> 20); // Registrador de destino
        imm = (mbr & 0x0000ffff); // Valor imediato

        // Move os dezesseis bits menos significativos do imediato para a parte superior do registrador X
        // Mantém os bits menos significativos do registrador X intactos
        move_immediate_upper(&reg[ro0], imm);

        pc += 4; // Incrementa o PC
    } else if (ir == 0b10010) {
        // ADD IMMEDIATE instruction
        ro0 = ((mbr & 0xf00000) >> 20); // Registrador de destino
        imm = (mbr & 0x0000ffff); // Valor imediato

        // Realiza a adição imediata
        add_immediate(&reg[ro0], imm);

        pc += 4; // Incrementa o PC
    } else if (ir == 0b10011) {
        // SUBTRACT IMMEDIATE instruction
        ro0 = ((mbr & 0xf00000) >> 20); // Registrador de destino
        imm = (mbr & 0x0000ffff); // Valor imediato

        // Realiza a subtração imediata
        subtract_immediate(&reg[ro0], imm);

        pc += 4; // Incrementa o PC
    } else if (ir == 0b10100) {
        // MULTIPLY IMMEDIATE instruction
        ro0 = ((mbr & 0xf00000) >> 20); // Registrador de destino
        imm = (mbr & 0x0000ffff); // Valor imediato

        // Realiza a multiplicação imediata
        multiply_immediate(&reg[ro0], imm);

        pc += 4; // Incrementa o PC
    } else if (ir == 0b10101) {
        // DIVIDE IMMEDIATE instruction
        ro0 = ((mbr & 0xf00000) >> 20); // Registrador de destino
        imm = (mbr & 0x0000ffff); // Valor imediato

        // Realiza a divisão imediata
        divide_immediate(&reg[ro0], imm);

        pc += 4; // Incrementa o PC
    } else if (ir == 0b10110) {
        // LEFT SHIFT instruction
        ro0 = ((mbr & 0xf00000) >> 20); // Registrador de destino
        imm = (mbr & 0x0000ffff); // Valor imediato

        // Desloca a palavra no registrador X em IMM bits à esquerda
        left_shift(&reg[ro0], imm);

        pc += 4; // Incrementa o PC
    } else if (ir == 0b10111) {
        // RIGHT SHIFT instruction
        ro0 = ((mbr & 0xf00000) >> 20); // Registrador de destino
        imm = (mbr & 0x0000ffff); // Valor imediato

        // Desloca a palavra no registrador X em IMM bits à direita
        right_shift(&reg[ro0], imm);

        pc += 4; // Incrementa o PC
    } else if (ir == 0b11000) {
        // JUMP IF EQUAL TO instruction
        imm = (mbr & 0x00ffffff); // Endereço de memória

        // Muda o registrador PC para o endereço de memória X caso E = 1
        if (e == 1) {
            pc = imm;
        } else {
            pc += 4; // Incrementa o PC
        }
    } else if (ir == 0b11001) {
        // JUMP IF NOT EQUAL TO instruction
        imm = (mbr & 0x00ffffff); // Endereço de memória

        // Muda o registrador PC para o endereço de memória X caso E = 0
        if (e == 0) {
            pc = imm;
        } else {
            pc += 4; // Incrementa o PC
        }
    } else if (ir == 0b11010) {
        // JUMP IF LOWER THAN instruction
        imm = (mbr & 0x00ffffff); // Endereço de memória

        // Muda o registrador PC para o endereço de memória X caso L = 1
        if (l == 1) {
            pc = imm;
        } else {
            pc += 4; // Incrementa o PC
        }
    } else if (ir == 0b11011) {
        // JUMP IF LOWER THAN OR EQUAL TO instruction
        imm = (mbr & 0x00ffffff); // Endereço de memória

        // Muda o registrador PC para o endereço de memória X caso E = 1 ou L = 1
        if (e == 1 || l == 1) {
            pc = imm;
        } else {
            pc += 4; // Incrementa o PC
        }
    } else if (ir == 0b11100) {
        // JUMP IF GREATER THAN instruction
        imm = (mbr & 0x00ffffff); // Endereço de memória

        // Muda o registrador PC para o endereço de memória X caso G = 1
        if (g == 1) {
            pc = imm;
        } else {
            pc += 4; // Incrementa o PC
        }
    } else if (ir == 0b11101) {
        // JUMP IF GREATER THAN OR EQUAL TO instruction
        imm = (mbr & 0x00ffffff); // Endereço de memória

        // Muda o registrador PC para o endereço de memória X caso E = 1 ou G = 1
        if (e == 1 || g == 1) {
            pc = imm;
        } else {
            pc += 4; // Incrementa o PC
        }
    } else if (ir == 0b11110) {
        // JUMP instruction
        imm = (mbr & 0x00ffffff); // Endereço de memória

        // Muda o registrador PC para o endereço de memória X
        pc = imm;
    }
}

unsigned char *pt;

void guardarMemoria(unsigned int mem ,unsigned int aux){
    memoria[mem++] = (aux & 0xff000000) >> 24;
    memoria[mem++] = (aux & 0x00ff0000) >> 16;
    memoria[mem++] = (aux & 0x0000ff00) >> 8;
    memoria[mem++] = aux & 0x000000ff;
}

int main() {
    lerArquivo();

    int teste = 0;
    int aux = 0;

    while (aux != 3){
        printf(" Aperte 1 para executar, 2 para mostrar a memoria, 3 para finalizar");
        scanf("%d", &aux);

        if (aux == 1) {
            ciclo();
            printf("MBR: %x MAR: %x  IMM: %x PC: %x \n",mbr, mar, imm,pc);
            printf("REG1: %x REG2: %x REG3: %x  REG4: %x \n", reg[0], reg[1], reg[2], reg[3]);
            printf("REG5: %x  REG6: %x  REG7: %x  REG8: %x \n", reg[4], reg[5], reg[6], reg[7]);
            printf("IR: %x  RO0: %x  RO1: %x  E: %x  L: %x G: %x \n", ir, ro0, ro1, e, l, g);
            if (ir == 0) {
                while(teste < 154){
                    printf("Posicao %d : %x \n", teste, memoria[teste++]);
                };
                break;
            }
        } else if (aux == 2) {
            while(teste < 154){
                printf("Posicao %d : %x \n", teste, memoria[teste]);
                teste++;
            };
        } else {
            break;
        }
    };

    printf("MBR: %x MAR: %x  IMM: %x PC: %x \n",mbr, mar, imm,pc);
    printf("REG1: %x REG2: %x REG3: %x  REG4: %x \n", reg[0], reg[1], reg[2], reg[3]);
    printf("REG5: %x  REG6: %x  REG7: %x  REG8: %x \n", reg[4], reg[5], reg[6], reg[7]);
    printf("IR: %x  RO0: %x  RO1: %x  E: %x  L: %x G: %x \n", ir, ro0, ro1, e, l, g);

    return 0;
}