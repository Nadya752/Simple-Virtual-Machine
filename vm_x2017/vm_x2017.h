#ifndef VM_H
#define VM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "disassembler_x2017.h"

#define MAX_INS (16)
#define LAB_CHAR (12)
#define MAX_RAM (256)
#define STK_FRM (32)

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

char* fetch (char* [], unsigned int*);
int get_op_code(const char*);
int get_type_code(const char*);
void call(x2017_cpu*, unsigned int, long);
void ret(x2017_cpu*);
void move(x2017_cpu*, unsigned int, unsigned int, unsigned int, unsigned int);
void reference(x2017_cpu*, unsigned int, unsigned int, unsigned int, unsigned int);
void add(x2017_cpu*, unsigned int, unsigned int, unsigned int, unsigned int);
void print_val(x2017_cpu*, unsigned int, unsigned int);
void bitwise_not(x2017_cpu*, unsigned int);
void equ(x2017_cpu*, unsigned int);
int init_pc(char* [], long);
void eval(x2017_cpu*, char*, long);

#endif