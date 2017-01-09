#include "HGeneration.h"

void Generation(long int N) {
	FILE *gfile;
	fopen_s(&gfile, "test.txt", "w");
	int i;
	char* s = (char *)malloc((N) * sizeof(char)); 
	srand(time(0));
	for (i = 0; i < N; i++) {
		//	rand() % 5 ? s[i] = 'a' + rand() % 26 : s[i] = ' ';
		s[i] = 'a' + rand() % 26;
		putc(s[i], gfile);
	}
	//fprintf(gfile, "%s", s);
	fclose(gfile);
	free(s);
}
void Initial_dictonary(unsigned int * code_value, unsigned int * p_prefix_code, unsigned char *p_append_character) {
	int i = 0,  j = 0;
	for (int k = 0; k < 26; k++) {
		p_prefix_code[k] = 'a' + i;
		p_append_character[k] = 'a' + i;
	}
	p_prefix_code[27] = ' ';
	p_append_character[27] = ' ';
	
	for (i = 1; i < 27;i++) {
		code_value[i] = code_value[i-1] + 1;
	}
}
