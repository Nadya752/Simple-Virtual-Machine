#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "disassembler_x2017.h"

int main(int argc, char* argv[]){
    // If there is no argument, then program exits.
    if (argc != ARG_NUM){
        exit(1);
    }
    char filename[MAX_NAME];
    strcpy(filename, argv[1]);

    // Getting the binary file.
    unsigned char buffer[BUF_SIZE];
    long len_bin = get_bin(filename, buffer);

    // For unformatted instructions.
    char* asm_instr[BUF_SIZE];
    for(int idx = 0; idx< BUF_SIZE; idx++){
        asm_instr[idx] = calloc(BUF_SIZE, sizeof(char));
    }

    // For formatted instructions.
    char* program_code[BUF_SIZE];
    for(int idx = 0; idx< BUF_SIZE; idx++){
        program_code[idx] = calloc(BUF_SIZE, sizeof(char));
    }

    // Disassembling the binary.
    long len_instr = decode(buffer, len_bin, asm_instr);
    // Formating into assembly.
    long len_prog = format_assembly(asm_instr, len_instr, program_code);
    //Translating the stack symbols.
    format_symbol(program_code, len_prog, 1);
    // Printing the assembly instructions.
    print_assembly(program_code, len_prog);

    for (int idx = 0; idx < BUF_SIZE; idx++){
        free(asm_instr[idx]);
    }
    
    for (int idx = 0; idx < BUF_SIZE; idx++){
        free(program_code[idx]);
    }

    return 0;
}
