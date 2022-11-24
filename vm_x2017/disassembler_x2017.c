#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HEX_BASE (16)
#define BUF_SIZE (1552)
#define ASM_SIZE (2056) 
#define MAX_OP (6) // OP with maximum letter is PRINT.
#define MAX_TYP (4) 
#define CODE_SIZE (256)
#define CHAR_MAX (16) // Max char in an instruction is when calling MOV, REF, or ADD.
#define BITS_NUM (8)
#define OP_NUM (8)
#define MAX_NAME (4096)
#define ARG_NUM (2)
#define MASK_8 (255)
#define MASK_5 (31)
#define MASK_3 (7)
#define MASK_2 (3)
#define STK_FRM (32)
#define MAX_INS_ARGS (2)

typedef enum {
    VAL_BIT = 8,
    INS_NUM_BIT = 5,
    STK_BIT = 5,
    PTR_BIT = 5,
    OP_BIT = 3,
    REG_BIT = 3,
    FUNC_LAB_BIT = 3,
    TYPE_BIT = 2
} bit_num;

typedef enum {
    VALUE,
    FUNC,
    STACK,
    PNTR,
    OP,
    REGIS,
    FUNC_LAB,
    TYPE
}instr_decode; 

typedef enum{
    MOV,
    CAL,
    RET,
    REF,
    ADD,
    PRINT,
    NOT,
    EQU,
} opcode;

typedef enum{
    VAL,
    REG,
    STK,
    PTR,
    TYP_NUM
} typecode;

// Opens binary file and get the binaries.
long get_bin(const char* filename, unsigned char* buffer){

    FILE* fp;
    fp = fopen(filename, "rb");
    // Terminates if file not found.
    if (fp == NULL){
        perror("The file does not exist.");
        exit(1);
    }

    fseek(fp, 0L, SEEK_END);
  
    // Calculating the size of the file
    long int size = ftell(fp);
    rewind(fp);
    // Put the binary to buffer.
    fread(buffer, sizeof(char), size, fp);
    fclose(fp);
    return size;

}

// Translate symbols based on number.
char get_symbol(int num){
    char symbol = 0;

    while (num <= STK_FRM){
        const char symbols[] = {"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefg"};
        symbol = symbols[num];
        return symbol;
    }

    return symbol;
}

// Decodes the binary.
long decode(unsigned char* buffer, long len, char* instr[BUF_SIZE]){
    const char* types[] = {"VAL", "REG", "STK", "PTR"};
    const char* op_codes[] = {"MOV", "CAL", "RET", "REF", "ADD", "PRINT", "NOT", "EQU"};

    unsigned char decode_8 = MASK_8;
    unsigned char decode_5 = MASK_5;
    unsigned char decode_3 = MASK_3;
    unsigned char decode_2 = MASK_2;

    long count_op = -1; // To count how many instructions left.
    long instr_idx = 0; // Index of instr array.
    long next_decode = 0; 
    long next_bit_num = 0;
    long current_op = 1; // Number of arguments in an op code. Can be 0, 1, or 2.
    long count_rem = 0; // Bits not decoded
    long p_bits = 0; // Bits already decoded.
    unsigned char remainder = 0;

    for(int idx = len-1; idx >=0; idx--){

        unsigned char ori_byte = (buffer[idx] << count_rem) ^ remainder;
        unsigned char byte = buffer[idx];
        long not_decoded = BITS_NUM - p_bits;

        while (not_decoded >= next_bit_num){
            byte = ori_byte >> (BITS_NUM - not_decoded);

            // In the beginning of instruction binary is the instruction number.
            if (count_op == -1){
                byte = byte & decode_5;
                count_op =  byte;
                next_bit_num = OP_BIT;
                not_decoded -= INS_NUM_BIT;
                next_decode = OP;

                sprintf(instr[instr_idx], "END");
                instr_idx++;

            // Decodes OP. In the beginning of every instruction is OP code.
            }else{
                if (next_decode == OP){
                    byte = byte & decode_3;

                    sprintf(instr[instr_idx], "%s", op_codes[byte]);
                    instr_idx++;
                    count_op--;
                    not_decoded -= OP_BIT;
                    if (byte != RET){
                        next_bit_num = TYPE_BIT;
                        next_decode = TYPE;
                        if (byte == MOV || byte == REF || byte == ADD){
                            current_op = MAX_INS_ARGS;
                        }

                    }else{
                        next_bit_num = OP_BIT;
                        next_decode = OP;
                    }
                // After OP or VAL is decoding TYPE.
                }else if (next_decode == TYPE){
                    byte = byte & decode_2;

                    sprintf(instr[instr_idx], "%s", types[byte]);
                    instr_idx++;
                    not_decoded -= TYPE_BIT;
                    if (byte == VAL){
                        next_bit_num = VAL_BIT;
                        next_decode = VALUE;
                    }else if (byte == REG){
                        next_bit_num = REG_BIT;
                        next_decode = REGIS;
                    }else if (byte == STK){
                        next_bit_num = STK_BIT;
                        next_decode = STACK;
                    }else if (byte == PTR){
                        next_bit_num = PTR_BIT;
                        next_decode = PNTR;
                    }

                // If TYPE is immediate value.
                }else if (next_decode == VALUE){
                    byte = byte & decode_8;

                    sprintf(instr[instr_idx], "%d", byte);
                    instr_idx++;
                    not_decoded -= VAL_BIT;

                    if (current_op == MAX_INS_ARGS){
                        next_bit_num = TYPE_BIT;
                        next_decode = TYPE;
                        current_op--;
                        
                    }else{
                        if (count_op >0){
                            next_bit_num = OP_BIT;
                            next_decode = OP;
                        }else{
                            next_bit_num = FUNC_LAB_BIT;
                            next_decode = FUNC_LAB;
                        }

                    }

                // If TYPE is STK or PNTR.
                }else if (next_decode == STACK || next_decode == PNTR){
                    byte = byte & decode_5;

                    sprintf(instr[instr_idx], "%d", byte);
                    
                    instr_idx++;
                    not_decoded -= STK_BIT;

                    if (current_op == MAX_INS_ARGS){
                        next_bit_num = TYPE_BIT;
                        next_decode = TYPE;
                        current_op--;
                        
                    }else{
                        if (count_op >0){
                            next_bit_num = OP_BIT;
                            next_decode = OP;
                        }else{
                            next_bit_num = FUNC_LAB_BIT;
                            next_decode = FUNC_LAB;
                        }
                    }

                // IF TYPE is register, or if next is a new function.
                }else if (next_decode == REGIS || next_decode == FUNC_LAB){
                    byte = byte & decode_3;

                    sprintf(instr[instr_idx], "%d", byte);
                    instr_idx++;
                    not_decoded -= REG_BIT;
                    if (next_decode == FUNC_LAB){
                        //end of binary file.
                        if (idx == (0)){
                            current_op = -1; // signalling to stop.
                            break;
                        }else{
                            //encode a new function.
                            next_bit_num = INS_NUM_BIT;
                            count_op = -1;
                        }
                    }else{
                        if (current_op == MAX_INS_ARGS){
                            next_bit_num = TYPE_BIT;
                            next_decode = TYPE;
                            current_op--;
                            
                        }else{
                            if (count_op >0){
                                next_bit_num = OP_BIT;
                                next_decode = OP;
                            }else{
                                next_bit_num = FUNC_LAB_BIT;
                                next_decode = FUNC_LAB;
                            }
                        }

                    }

                }

            }
        }

        // Saves the reminder of the bits that could not be deoded 
        // because it needs the next byte.
        if (count_rem == 0){
            count_rem = not_decoded;
            remainder = ori_byte >> (BITS_NUM - not_decoded);
            p_bits = 0;

        // Return to the original form of the byte without adding remainder.
        }else if (count_rem > 0 && !(idx == 0 && current_op == -1)){
            p_bits = BITS_NUM - (count_rem + not_decoded);
            count_rem = 0;
            remainder = 0;
            idx++;
        }
    }

    return instr_idx;
}

// Checks if instruction is OP code.
int not_opcode(char* instr_str){
    int not_op = 1;
    const char* op_codes[] = {"MOV", "CAL", "RET", "REF", "ADD", "PRINT", "NOT", "EQU"};

    for (int idx = 0; idx < OP_NUM; idx++){
        if (!(strcmp(instr_str, op_codes[idx]))){
            not_op = 0;
            break;
        }
    }
    return not_op;

}

// Formats instruction and value into assembly.
long format_assembly(char* instr[BUF_SIZE], long len, char* program_code[BUF_SIZE]){

    int new_func = 1;
    long prog_idx = 0;

    for(int idx = (len-1); idx>=0; idx--){
        // If its the beginning of function.
        if (new_func){
            sprintf(program_code[prog_idx], "FUNC LABEL %c", instr[idx][0]);
            prog_idx++;
            new_func = 0;

        // End of function after ret.
        }else if (strcmp(instr[idx], "END") == 0){
            if(idx == 0){
                break;
            }else{
                new_func = 1;
                continue;
            }

        //OP Codes.
        }else{
            long op_idx = 0;
            long end_idx = idx;

            while (not_opcode(instr[idx])){
                idx--;
            }

            // op idx is where op code is
            // end idx is the original idx
            op_idx = idx;
            while(op_idx <= end_idx){
                
                strcat(program_code[prog_idx], instr[op_idx]);

                if (op_idx != end_idx){
                    strcat(program_code[prog_idx], " ");
                }

                op_idx++;
                
            }

            prog_idx++;
            
        }
    }
    
    return prog_idx;

}

// Formats symbol. Print mode shows the alphabetical symbols, else its just numbers for VM.
void format_symbol(char* program_code[],long len_prog, int print_mode){

    unsigned int sym_map[STK_FRM];
    int len_map = 0;

    for (int idx = 0; idx < len_prog; idx++){
        char op[MAX_OP];
        char type_1[MAX_TYP];
        char type_2[MAX_TYP];
        unsigned int value_1 = 0;
        unsigned int value_2 = 0;
        int change_1 = 0;
        int change_2 = 0;

        // Every time it encounters new function, all the symbols goes back to A (0).
        if(strncmp(program_code[idx], "FUNC LABEL", strlen("FUNC LABEL")) == 0){
            len_map = 0;
            memset(sym_map, '\0', STK_FRM*sizeof(int));
            continue;
        }

        // Searches for PTR, STK, and PRINT which uses symbols (PRINT could print a STK or PTR).
        if (strncmp(program_code[idx], "MOV", strlen("MOV")) == 0 || strncmp(program_code[idx], "REF", strlen("REF")) == 0){
            sscanf(program_code[idx], "%s %s %d %s %d", op, type_1, &value_1, type_2, &value_2);
            if(strcmp(type_1, "STK") == 0 || strcmp(type_1, "PTR") == 0){
                change_1 = 1;
                int exist = 0;
                for ( int idx = 0; idx<len_map; idx++){
                    if(sym_map[idx] == value_1){
                        exist = 1;
                        value_1 = idx;
                        break;
                    }
                }
                
                if(!(exist)){
                    sym_map[len_map] = value_1;
                    value_1 = len_map;
                    len_map++;

                }

            }

            if(strcmp(type_2, "STK") == 0 || strcmp(type_2, "PTR") == 0){
                change_2 = 1;
                int exist = 0;
                for ( int idx = 0; idx<len_map; idx++){
                    if(sym_map[idx] == value_2){
                        exist = 1;
                        value_2 = idx;
                        break;

                    }
                }
                
                if(!(exist)){
                    sym_map[len_map] = value_2;
                    value_2 = len_map;
                    len_map++;

                }

            }


            if (print_mode){
                if (change_1 && change_2){
                    sprintf(program_code[idx], "%s %s %c %s %c", op, type_1, get_symbol(value_1), type_2, get_symbol(value_2));

                }else if (change_1 && !(change_2)){
                    sprintf(program_code[idx], "%s %s %c %s %d", op, type_1, get_symbol(value_1), type_2, value_2);
 
                }else if (!(change_1) && change_2){
                    sprintf(program_code[idx], "%s %s %d %s %c", op, type_1, value_1, type_2, get_symbol(value_2));
                }

            }else{
                sprintf(program_code[idx], "%s %s %d %s %d", op, type_1, value_1, type_2, value_2);

            }

        }else if(strncmp(program_code[idx], "PRINT", strlen("PRINT")) == 0 ){
            sscanf(program_code[idx], "%s %s %d", op, type_1, &value_1);
            if(strcmp(type_1, "STK") == 0 || strcmp(type_1, "PTR") == 0){
                for (int idx = 0; idx<len_map; idx++){
                    if(sym_map[idx] == value_1){
                        value_1 = idx;
                        break;

                    }
                }
                if (print_mode){
                    sprintf(program_code[idx], "%s %s %c", op, type_1, get_symbol(value_1));
                }else{
                    sprintf(program_code[idx], "%s %s %d", op, type_1, value_1);
                }

            }        
        }
    }
}

// Prints the instruction.
void print_assembly(char* program_code[],long len_prog){
    for (int idx = 0; idx < len_prog; idx++){
        // If its not function label, give 4 space as a indent.
        if (strncmp(program_code[idx], "FUNC LABEL", strlen("FUNC LABEL"))){
            printf("    ");
        }

        printf("%s", program_code[idx]);
        printf("\n");
    }

}