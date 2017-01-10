#ifndef _FUNCTION_H_
#define _FUNCTION_H_

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"


#define BITS 12
#define HASHING_SHIFT 4 
#define MAX_VALUE (1 << BITS) - 1
#define MAX_CODE MAX_VALUE - 1   
#define TABLE_SIZE 4096

extern unsigned char decode_stack[4095];
extern unsigned int code_value[TABLE_SIZE];
extern unsigned int prefix_code[TABLE_SIZE];
extern unsigned char append_character[TABLE_SIZE];

void Generation(long int N);
unsigned char *decode_string(unsigned char *buffer, unsigned int code);
void compress(FILE *input, FILE *output);
void expand(FILE *input, FILE *output);

int find_match(int hash_prefix, unsigned int hash_character);
void output_code(FILE *output, unsigned int code);
unsigned int input_code(FILE *input);
#endif