#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "disassembler_x2017.h"

#define MAX_INS (16)
#define LAB_CHAR (12)
#define MAX_RAM (256)
#define EXTRA_FRM (2)
#define MAX_OP (6) // OP with maximum letter is PRINT.
#define MAX_TYP (4)

typedef enum{
    R0,
    R1, 
    R2,
    R3,
    RA, // Return address.
    SP, // Stack pointer.
    STATE, // State of cpu, wheter its RUN or HALT.
    PC, // Progam counter.
    REG_NUM
}reg_name;

typedef enum{
    HALT,
    RUN
} cpu_state;

typedef struct {

    char** program_code;
    unsigned int* registers;
    unsigned int* RAM;

} x2017_cpu;

// Fetches instructions one by one.
char* fetch (char* program_code[], unsigned int* pc){

    char* instruction = program_code[*pc];
    ++(*pc);
    return instruction;
}

// Returns the op_code based on the operations.
int get_op_code(const char* op){

    int code = -1;
    const char* op_codes[] = {"MOV", "CAL", "RET", "REF", "ADD", "PRINT", "NOT", "EQU"};

    for (int idx = 0; idx < OP_NUM; idx++){
        if (!(strcmp(op, op_codes[idx]))){
            code = idx;
            break;
        }
    }

    return code;

}

//Retuns the type-code based on types.
int get_type_code(const char* type){
    int code = -1;
    const char* types[] = {"VAL", "REG", "STK", "PTR"};

    for (int idx = 0; idx < TYP_NUM; idx++){
        if (!(strcmp(type, types[idx]))){
            code = idx;
            break;
        }
    }

    return code;

}

// Calls another function.
void call(x2017_cpu* cpu,unsigned int value, long len_prog){
    char label[LAB_CHAR+1];
    sprintf(label, "FUNC LABEL %d", value);
    unsigned int idx = 0;
    int found = 0;

    // Search for the label of function that is going to be called.
    while(idx<len_prog){

        if(strcmp(cpu->program_code[idx], label) == 0){
            found = 1;
            break;
        }
        
        idx++;
    }

    if (!found){

        cpu->registers[STATE] = HALT;
        return;
    }

    // Just in case for nested function calls, 
    // the return address is put in the last stack in the previous stack frame before creating a new one.
    // while the current return address is saved in the RA register.
    if (cpu->registers[SP] >=1){

        unsigned int last_symbol = (STK_FRM-1) + (STK_FRM*((cpu->registers[SP])-1));
        cpu->RAM[last_symbol] = cpu->registers[RA];

    }

    // Updates return address, program counter, and stack pointer.
    cpu->registers[RA] = (cpu->registers[PC]);
    cpu->registers[PC] = idx+1;
    cpu->registers[SP] = (cpu->registers[SP])+ 1;

}

// Returns to previous function, or terminates program of function is label 0.
void ret(x2017_cpu* cpu){
    // Terminates program when function label 0 returns.
    if (cpu->registers[SP] == 0){
        cpu->registers[STATE] = HALT;
    }else{

        // Pops the stack of the returning function.
        memset((cpu->RAM) + (STK_FRM*((cpu->registers[SP]))), '\0', STK_FRM*sizeof(unsigned int));
        cpu->registers[PC] = cpu->registers[RA];

        // Assign RA to the next return address if it is nested,
        // or NULL if its just a one function call.
        if (cpu->registers[SP] >1 ){
            unsigned int last_symbol = (STK_FRM-1) + (STK_FRM*((cpu->registers[SP])-EXTRA_FRM));
            cpu->registers[RA] = cpu->RAM[last_symbol];

        }else if (cpu->registers[SP] == 1 ){
            cpu->registers[RA] = 0;
        }

        cpu->registers[SP] = (cpu->registers[SP]) - 1;
    }

}

// Moves value.
void move(x2017_cpu* cpu, unsigned int type_1, unsigned int type_2, unsigned int value_1, unsigned int value_2){


    unsigned int arg2;

    switch (type_2){
        case VAL:
            arg2 = value_2;
            break;

        case REG:
            arg2 = cpu->registers[value_2];
            break;

        case STK:
        {   
            int offset = cpu ->registers[SP]*STK_FRM;
            arg2 = cpu ->RAM[value_2 + offset];
            break;
        }

        case PTR:
        {
            int offset = cpu ->registers[SP]*STK_FRM;
            int symbol = cpu ->RAM[value_2 + offset];
            arg2 = cpu->RAM[symbol];

            break;
        }

    }

    switch (type_1){
        case VAL:
            // Can't move a value into a value.
            return;
        case REG:
        {
            cpu->registers[value_1] = arg2;
            break;
        }

        case STK:
        {
            int offset = cpu ->registers[SP]*STK_FRM;
            cpu ->RAM[value_1 + offset] = arg2;
            break;
        }

        case PTR:
        {
            int offset = cpu ->registers[SP]*STK_FRM;
            int symbol = cpu ->RAM[value_1 + offset];
            cpu->RAM[symbol] = arg2;
            break;
        }

    }
}

// References stack address.
void reference(x2017_cpu* cpu, unsigned int type_1, unsigned int type_2, unsigned int value_1, unsigned int value_2){

    unsigned int arg2;
    switch (type_2){

        case STK:
        {   
            int offset = cpu ->registers[SP]*STK_FRM;
            arg2 = value_2 + offset;
            break;
        }

        case PTR:
        {
            int offset = cpu ->registers[SP]*STK_FRM;
            int symbol = cpu ->RAM[value_2 + offset];
            arg2 = symbol;
            break;
        }

    }
    switch(type_1){
        case VAL:
            // Can't move stack address to a value.
            return;

        case REG:
            cpu->registers[value_1] = arg2;
            break;
        case STK:
        {
            int offset = cpu ->registers[SP]*STK_FRM;
            cpu ->RAM[value_1 + offset] = arg2;
            break;
        }

        case PTR:
        {
            int offset = cpu ->registers[SP]*STK_FRM;
            int symbol = cpu ->RAM[value_1 + offset];
            cpu->RAM[symbol] = arg2;
            break;
        }


    }

}

// Does addition operation.
void add(x2017_cpu* cpu, unsigned int type_1, unsigned int type_2, unsigned int value_1, unsigned int value_2){
    // Does not do anything if addition is not from two registers.
    if (type_1 == REG && type_2 == REG){
        u_int8_t a = cpu->registers[value_1];
        u_int8_t b = cpu->registers[value_2];
        u_int8_t total = a + b;
        cpu->registers[value_1] = total;
    }

}

// Prints value.
void print_val(x2017_cpu* cpu, unsigned int type, unsigned int value){

    switch(type){
        case VAL:
            printf("%u\n", value);
            break;
        case REG:
            printf("%u\n", cpu->registers[value]);
            break;
        case STK:
        {
            int offset = cpu->registers[SP]*STK_FRM;
            printf("%u\n", cpu->RAM[value+offset]);
            break;
        }
        case PTR:
        {
            int offset = cpu ->registers[SP]*STK_FRM;
            int symbol = cpu ->RAM[value + offset];
            printf("%u\n", cpu->RAM[symbol]);  
            break;          
        }
    }

}

// Does bitwise not (and subtraction).
void bitwise_not(x2017_cpu* cpu, unsigned int value){

    int8_t number = cpu->registers[value];
    int8_t not_number = ~number;
    cpu->registers[value] = (int)not_number;

}

// Compares value against 0.
void equ(x2017_cpu* cpu, unsigned int value){
    if (cpu->registers[value] == 0){
        cpu->registers[value] = 1;
    }else{
        cpu->registers[value] = 0;
    }
}

// Tells the program counter where to start.
int init_pc(char* program_code[], long len_prog){

    // Searches for function label 0.
    char label[LAB_CHAR+1];
    sprintf(label, "FUNC LABEL 0");
    unsigned int idx = 0;
    int found = 0;

    while(idx < len_prog){
        if(strcmp(program_code[idx], label) == 0){
            found = 1;
            break;
        }
        idx++;
    }

    // If function label 0 not existent, -1 is returned.
    if (found){
        return idx+1;
    }
    return -1;
}

// Runs the instruction one by one according to OP code.
void eval(x2017_cpu* cpu, char* instruction, long len_prog){

    char op[MAX_OP];
    char type_1[MAX_TYP];
    char type_2[MAX_TYP];
    unsigned int value_1 = 0;
    unsigned int value_2 = 0;
    
    sscanf(instruction, "%s", op);
    int op_code = get_op_code(op);
    int typ1_code = 0;
    int typ2_code = 0;

    if(op_code == MOV || op_code == REF || op_code == ADD){
        sscanf(instruction, "%*s %s %d %s %d", type_1, &value_1, type_2, &value_2);
        typ1_code = get_type_code(type_1);
        typ2_code = get_type_code(type_2);
    }else if (op_code != RET){
        sscanf(instruction, "%*s %s %d", type_1, &value_1);
        typ1_code = get_type_code(type_1);
    }

    // If the last instruction is not RET, then program terminates.
    // and if the function has no RET, then the next instruction would be 
    // a function label, which is not an op code (-1), and will go to the default case.
    if (cpu->registers[PC] == len_prog && op_code != RET){
        cpu->registers[STATE] = HALT;
        return;
    }

    // Executing the op code.
    switch(op_code){
        case MOV:
            move(cpu, typ1_code, typ2_code, value_1, value_2);
            break;

        case CAL:
            call(cpu, value_1, len_prog);
            break;

        case RET:
            ret(cpu);
            break;

        case REF:
            reference(cpu, typ1_code, typ2_code, value_1, value_2);
            break;

        case ADD:
            add(cpu, typ1_code, typ2_code, value_1, value_2);
            break;

        case PRINT:
            print_val(cpu, typ1_code, value_1);
            break;

        case NOT:
            bitwise_not(cpu, value_1);
            break;

        case EQU:
            equ(cpu, value_1);
            break;

        default:
            printf("Invalid OP code %d on instruction \"%s\".", op_code, instruction);
            cpu->registers[STATE] = HALT;
            break;
    }

}

int main(int argc, char* argv[]){
    // If there is no argument, then program exits.
   if (argc != ARG_NUM ){
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
    format_symbol(program_code, len_prog, 0);

    unsigned int reg[REG_NUM];
    memset(reg, '\0', REG_NUM*sizeof(int));

    unsigned int stack[MAX_RAM];
    memset(stack, '\0', MAX_RAM*sizeof(int));

    // Initialising the cpu.
    x2017_cpu cpu = {.program_code = program_code, .registers = reg, .RAM = stack};
    cpu.registers[STATE] = RUN;

    // Search for starting point (function label 0)
    int start = init_pc(program_code, len_prog);
    // If there is no function label 0, program terminates.
    if (start == -1){
        cpu.registers[STATE] = HALT;
    }else{
        cpu.registers[PC] = start;
    }

    // Running the instructions until finish.
    while(cpu.registers[STATE] == RUN){
        char* instruction = fetch(cpu.program_code, &(cpu.registers[PC]));
        eval(&cpu, instruction, len_prog);
    }

    for (int idx = 0; idx < BUF_SIZE; idx++){
        free(asm_instr[idx]);
    }
    
    for (int idx = 0; idx < BUF_SIZE; idx++){
        free(program_code[idx]);
    }

    return 0;
}
