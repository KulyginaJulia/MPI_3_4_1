#ifndef _HEADER_H_
#define _HEADER_H_
#define BITS 12
#define HASHING_SHIFT 4 
#define MAX_VALUE (1 << BITS) - 1
#define MAX_CODE MAX_VALUE - 1   
#define TABLE_SIZE 4096 // 2^12

#include "Function.h"

char * output_code(char *output, unsigned int code, int count, int &length_out);
void Testing(FILE* A, FILE* B, int count);
unsigned int find_match(unsigned int hash_prefix, unsigned int hash_character, unsigned int *temp_code_value, unsigned char* p_append_character, unsigned int* p_prefix_code);
char* compress(char *input, int count, char *output, int count_out, int &length_out);
char* expand(char *input, char *output, int lenght_out, int count);
unsigned int input_code(char *input, int length_out);
unsigned char *decode_string(unsigned char *buffer, unsigned int code, unsigned char* p_append_character, unsigned int* p_prefix_code);
#endif
