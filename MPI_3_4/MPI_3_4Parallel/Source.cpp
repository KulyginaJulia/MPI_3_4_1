#include "HGeneration.h"
#include "Header.h"

char * output_code(char *output, unsigned int code, int count)
{
	static int j = 0;
	static int output_bit_count = 0;
	static unsigned long output_bit_buffer = 0L;
	output_bit_buffer = output_bit_buffer | ((unsigned long)code << (32 - BITS - output_bit_count));
	output_bit_count += BITS;
	while ((output_bit_count >= 8) && (j < count) ){
			output[j] = output_bit_buffer >> 24;
		output_bit_buffer = output_bit_buffer << 8;
		output_bit_count -= 8;
		j++;
	}
	return output; 
}
char *compress(char *input, int count_input, char *output, int count_out, int & length_out, unsigned int* temp_code_value, unsigned char* p_append_character, unsigned int* p_prefix_code) {
	unsigned int next_code;
	unsigned int character;
	unsigned int string_code;
	unsigned int index;
	int i = 0;
	unsigned char p_decode_stack[4095];
	next_code = 256;

	for (i = 0; i < TABLE_SIZE; i++) {
		temp_code_value[i] = -1;
		p_prefix_code[i] = -1;
		p_append_character[i] = -1;
	}
	
	/*for (int k = 0; k < count_out; k++)
		output[k] = 0;*/
	i = 0;
	printf_s("Compressing...\n");
	string_code = input[0];
	i++;
	length_out = 0;
	while (i < count_input){
		character = input[i];
		index = find_match(string_code, character, temp_code_value, p_append_character,p_prefix_code);
		if (temp_code_value[index] != -1)
			string_code = temp_code_value[index];
		else
		{
			if (next_code <= MAX_CODE)
			{
				temp_code_value[index] = next_code++;
				p_prefix_code[index] = string_code;
				p_append_character[index] = character;
			}
			
			output = output_code(output, string_code,count_out);
			string_code = character;
		}
		i++;
	}
	output = output_code(output, string_code, count_out);
	output_code(output, MAX_VALUE, count_out);
	output_code(output, 0, count_out);
	for (int j = 0; j < count_out; j++)
		if (output[j] != -1)
			length_out++; //length_out++;

	return output;
}

unsigned int find_match(unsigned int hash_prefix, unsigned int hash_character, unsigned int *temp_code_value, unsigned char* p_append_character, unsigned int* p_prefix_code)
{
	int index;
	int offset;

	index = (hash_character << HASHING_SHIFT) ^ hash_prefix;
	offset = 1;
	bool flag = 0;
	while (flag != 1){
		if (temp_code_value[index] == -1)
			flag = 1;
		else {
			if ((p_prefix_code[index] == hash_prefix) && (p_append_character[index] == hash_character))
				flag = 1;
			else {
				index -= offset;
				if (index < 0)
					index += TABLE_SIZE;
			}
		}
	}
	//while (1)
	//{
	//	if (temp_code_value[index] == -1)
	//		return(index);
	//	if ((p_prefix_code[index] == hash_prefix) && (p_append_character[index] == hash_character))
	//		return(index);
	//	index -= offset;
	//	if (index < 0)
	//		index += TABLE_SIZE;
	//}
	return(index);
}

void Testing(FILE* A, FILE* B, int count) {
	int flag = 1;
	char* ch_1 = (char*)malloc(count * sizeof(char));
	char* ch_2 = (char*)malloc(count * sizeof(char));
	while (!feof(A) && !feof(B))
	{
		fgets(ch_1, count, A);
		fgets(ch_2, count, B);
		if (strcmp(ch_1, ch_2))
			flag = 0;																																							//flag = 1;
	}

	if (flag == 1)
		printf_s("Correct!");
	else
		printf_s("Uncorrect!");
	free(ch_1);
	free(ch_2);
}
char* expand(char *input, char *output, int length_out, unsigned char* p_append_character, unsigned int* p_prefix_code) {
	unsigned int next_code = 0;
	unsigned int new_code = 0;
	unsigned int old_code = 0;
	int character = 0;
	int counter = 0;
	unsigned char *string;
	int j = 0;
	next_code = 256;
	printf("Expanding...\n");
	old_code = input_code(input, length_out);
	character = old_code;
	output[j] = old_code;
	j++;
	while ((new_code = input_code(input, length_out)) != (MAX_VALUE)) {
		if (new_code >= next_code) {
			*decode_stack = character;
			string = decode_string(decode_stack + 1, old_code, p_append_character, p_prefix_code);
		}
		else
			string = decode_string(decode_stack, new_code, p_append_character, p_prefix_code);
		character = *string;
		while (string >= decode_stack) {
			output[j] = *string--;
			j++;
		}
		if (next_code <= MAX_CODE) {
			p_prefix_code[next_code] = old_code;
			p_append_character[next_code] = character;
			next_code++;
		}
		old_code = new_code;
	}
	return output;
}
unsigned int input_code(char *input, int length_out)
{
	unsigned int return_value;
	static int input_bit_count = 0;
	static unsigned long input_bit_buffer = 0L;
	static int i = 0;
	while ((input_bit_count <= 24) && (i<length_out)){
		input_bit_buffer = input_bit_buffer | (unsigned long)input[i] << (24 - input_bit_count);
		input_bit_count += 8;
		i++;
	}
	return_value = input_bit_buffer >> (32 - BITS);
	input_bit_buffer = input_bit_buffer << BITS;
	input_bit_count -= BITS;
	return(return_value);
}
unsigned char *decode_string(unsigned char *buffer, unsigned int code, unsigned char* p_append_character, unsigned int* p_prefix_code)
{
	int i = 0;
	while (code > 255) {
		*buffer++ = p_append_character[code];
		code = p_prefix_code[code];
	}
	*buffer = code;
	return buffer;
}