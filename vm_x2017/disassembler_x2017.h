#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HEX_BASE (16)
#define BUF_SIZE (1552)
#define ASM_SIZE (2056) 
#define WORD_MAX (6) // OP with maximum letter is PRINT.
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

typedef enum{
    VAL_BIT = 8,
    INS_NUM_BIT = 5,
    STK_BIT = 5,
    PTR_BIT = 5,
    OP_BIT = 3,
    REG_BIT = 3,
    FUNC_LAB_BIT = 3,
    TYPE_BIT = 2
}bit_num;

typedef enum{
    VALUE,
    FUNC,
    STACK,
    PNTR,
    OP,
    REGIS,
    FUNC_LAB,
    TYPE
}instr_type; 

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

long get_bin(const char*, unsigned char*);
long decode(unsigned char*, long, char* [BUF_SIZE]);
int not_opcode(char* );
long format_assembly(char* [BUF_SIZE], long, char*[BUF_SIZE]);
void format_symbol(char* [],long, int);
void print_assembly(char* [],long);

#endif
