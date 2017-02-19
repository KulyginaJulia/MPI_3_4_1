#include "Function.h"
void Generation(long int N) {
	FILE *gfile;
	fopen_s(&gfile, "test.txt", "w");
	int i;
	char* s = (char *)malloc((N) * sizeof(char));
	srand(time(0));
	for (i = 0; i < N; i++) {
		s[i] = 'a' + rand() % 26;
		putc(s[i], gfile);
	}
	fclose(gfile);
	free(s);
}
void compress(FILE *input, FILE *output){
	unsigned int next_code;
	unsigned int character;
	unsigned int string_code;
	unsigned int index;
	int i;
	next_code = 256;
	for (i = 0; i < TABLE_SIZE; i++)
		code_value[i] = -1;
	i = 0;
	printf_s("Compressing...\n");
	string_code = getc(input); 
	while ((character = getc(input)) != (unsigned)EOF)
	{
		index = find_match(string_code, character);
		if (code_value[index] != -1)
			string_code = code_value[index];
		else                              
		{                                
			if (next_code <= MAX_CODE)
			{
				code_value[index] = next_code++;
				prefix_code[index] = string_code;
				append_character[index] = character;
			}
			output_code(output, string_code); 
			string_code = character;
		}                                  
	}                                    
	output_code(output, string_code);
	output_code(output, MAX_VALUE);
	output_code(output, 0);
	printf("\n");
}

int find_match(int hash_prefix, unsigned int hash_character)
{
	int index;
	int offset;

	index = (hash_character << HASHING_SHIFT) ^ hash_prefix;
	offset = 1;
	bool flag = 0;
	while (flag != 1){
		if (code_value[index] == -1)
			flag = 1;
		else {
			if ((prefix_code[index] == hash_prefix) && (append_character[index] == hash_character))
				flag = 1;
			else {
				index -= offset;
				if (index < 0)
					index += TABLE_SIZE;
			}
		}
	} 
	/*while (1)
	{
		if (code_value[index] == -1)
			return(index);
		if ((prefix_code[index] == hash_prefix) && (append_character[index] == hash_character))
			return(index);
		index -= offset;
		if (index < 0)
			index += TABLE_SIZE;
	}*/
	return(index);
}

void expand(FILE *input, FILE *output) {
	unsigned int next_code=0;
	unsigned int new_code=0;
	unsigned int old_code=0;
	int character=0;
	int counter=0;
	unsigned char *string;

	next_code = 256;
	printf("Expanding...\n");
	old_code = input_code(input);
	character = old_code;
	putc(old_code, output);

	while ((new_code = input_code(input)) != (MAX_VALUE)){
		if (new_code >= next_code){
			*decode_stack = character;
			string = decode_string(decode_stack + 1, old_code);
		}
		else
			string = decode_string(decode_stack, new_code);
		character = *string;
		while (string >= decode_stack) {
			putc(*string--, output);
		}
		if (next_code <= MAX_CODE){
			prefix_code[next_code] = old_code;
			append_character[next_code] = character;
			next_code++;
		}
		old_code = new_code;
	}
}

unsigned char *decode_string(unsigned char *buffer, unsigned int code)
{
	int i = 0;
	while (code > 255){
		*buffer++ = append_character[code];
		code = prefix_code[code];
	}
	*buffer = code;
	return buffer;
}

unsigned int input_code(FILE *input)
{
	unsigned int return_value;
	static int input_bit_count = 0;
	static unsigned long input_bit_buffer = 0L;
	while (input_bit_count <= 24) {
		input_bit_buffer = input_bit_buffer | (unsigned long)getc(input) << (24 - input_bit_count);
		input_bit_count += 8;
	}
	return_value = input_bit_buffer >> (32 - BITS);
	input_bit_buffer = input_bit_buffer << BITS;
	input_bit_count -= BITS;
	return(return_value);

}
void output_code(FILE *output, unsigned int code)
{
	static int output_bit_count = 0;
	static unsigned long output_bit_buffer = 0L;
	output_bit_buffer = output_bit_buffer | ((unsigned long)code << (32 - BITS - output_bit_count));
	output_bit_count += BITS;
	while (output_bit_count >= 8){
		putc(output_bit_buffer >> 24, output);
		output_bit_buffer = output_bit_buffer << 8;
		output_bit_count -= 8;
	}
}