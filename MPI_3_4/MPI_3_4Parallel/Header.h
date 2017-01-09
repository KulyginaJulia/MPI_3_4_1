#ifndef _HEADER_H_
#define _HEADER_H_
#define BITS 12                  // Установка длины кода равной 12 битам.
#define HASHING_SHIFT 4 
#define MAX_VALUE (1 << BITS) - 1
#define MAX_CODE MAX_VALUE - 1   
#define TABLE_SIZE 4096 // 2^12

#include "Function.h"

extern unsigned int  code_value[TABLE_SIZE];
extern unsigned int prefix_code[TABLE_SIZE]; 
extern unsigned char append_character[TABLE_SIZE];
extern unsigned char decode_stack[4095];

char * output_code(char *output, unsigned int code, int count);
void Testing(FILE* A, FILE* B, int count);
unsigned int find_match(unsigned int hash_prefix, unsigned int hash_character, unsigned int *temp_code_value, unsigned char* p_append_character, unsigned int* p_prefix_code);
char* compress(char *input, int count, char *output, int count_out, int &length_out,  unsigned int *temp_code_value, unsigned char* p_append_character, unsigned int* p_prefix_code);


void bubblesort(unsigned int* A, int count);
unsigned int* Merge(unsigned int* A, unsigned int* B, int Count);

#endif
