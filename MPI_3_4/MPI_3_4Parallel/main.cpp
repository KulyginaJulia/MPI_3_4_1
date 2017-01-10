#include "mpi.h"
#include "Function.h"
#include "Header.h"
#include "string.h"

unsigned char decode_stack[4095];
unsigned int code_value[TABLE_SIZE];
unsigned int prefix_code[TABLE_SIZE];
unsigned char append_character[TABLE_SIZE];

int main(int argc, char *argv[]){
	int ProcNum, ProcRank, dataSize;
	double time1, time2, delta_time_1, delta_time_2, deltaSize;
	FILE *input_file;
	FILE *output_file;
	FILE *lzw_file;
	int Count;
	char *str_file = NULL;

	if (argc >= 2) {
		Count = atoi(argv[1]);
	}
	MPI_Status status;
	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
	str_file = (char*)malloc(Count * sizeof(char));
	dataSize = (int)(Count / (ProcNum));
	deltaSize = Count % (ProcNum);
	if (ProcRank == 0) {
		Generation(Count);

		fopen_s(&input_file, "test.txt", "rb");
		fopen_s(&lzw_file, "test.lzw", "wb");

		time1 = MPI_Wtime();

		compress(input_file, lzw_file);
		time2 = MPI_Wtime();

		delta_time_1 = time2 - time1;

		fclose(lzw_file);
		
		fopen_s(&lzw_file, "test.lzw", "rb");
		fopen_s(&output_file, "testout.txt", "wb");

		for (int i = 0; i < TABLE_SIZE; i++) {
			prefix_code[i] = -1;
			code_value[i] = -1;
			append_character[i] = 0;
		}
		expand(lzw_file, output_file);
		fclose(output_file);
		fclose(lzw_file);
		fopen_s(&output_file, "testout.txt", "rb");
		Testing(output_file, input_file, Count);

		fclose(output_file);
		fclose(input_file);
		
		printf_s("\n Consistent Time: %f", delta_time_1);
		printf_s("\n");
	}

	if (ProcRank == 0) {
		int i;
		int c = 3 * dataSize / 2;
		char *str_file = (char*)malloc((Count+1) * sizeof(char));
		char* out_str = (char*)malloc((c)*ProcNum * sizeof(char));
		char *str_file_t = (char*)malloc((Count + 1) * sizeof(char));
		char *tempv = str_file;	
		int * temp_out_length = (int*)malloc((ProcNum) * sizeof(int));
		char *tempv1 = str_file;
		
		for (int i = 0; i < c*ProcNum; i++)
			out_str[i] = -1;

		//compress
		fopen_s(&input_file, "test.txt", "rb");
		fgets(str_file, Count+1, input_file);
		fclose(input_file);

		time1 = MPI_Wtime();
		tempv += dataSize;

		for (i = 1; i < ProcNum; i++) {
			MPI_Send(tempv, dataSize, MPI_CHAR, i, 0, MPI_COMM_WORLD);
			tempv += dataSize;
		}
		char* temp_out = out_str;
		int length_out = 0;
		temp_out = compress(tempv1, dataSize, out_str, c, length_out);
		temp_out += length_out;
		temp_out_length[0] = length_out;
		for (i = 1; i < ProcNum; i++) {
			MPI_Recv(&temp_out_length[i], 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
		}

		for (int j = 0; j < ProcNum; j++) {
			printf_s("out_length[ %d", j);
			printf_s("] = %d", temp_out_length[j]);
			printf_s("\n");
		}

		for (i = 1; i < ProcNum; i++) {
			MPI_Recv(temp_out, temp_out_length[i], MPI_CHAR, i, 0, MPI_COMM_WORLD, &status);
			temp_out += temp_out_length[i];
		}	

		if (deltaSize != 0) {
			int j = Count - deltaSize;
			while (j < Count ) {
				for (int i = c*ProcNum; i < c*ProcNum + deltaSize; i++)
					out_str[i] = str_file[j];
				j++;
			}			
		}

		time2 = MPI_Wtime();

		fopen_s(&lzw_file, "Ptest.lzw", "wb");
		for (int j = 0; j < ProcNum; j++)
			for (i = 0; i < temp_out_length[j]; i++)
				putc(out_str[j*(temp_out_length[j]-1) +i], lzw_file);

		fclose(lzw_file);
		
		fopen_s(&output_file, "Ptestout.txt", "wb");
		//expand
		char* temp_str_1 = str_file_t;
		char* temp_out_1 = out_str;
		
		str_file_t = expand(out_str, str_file_t, temp_out_length[0],dataSize);

		char* temp_str = str_file_t;
		for (i = 1; i < ProcNum; i++) {
			temp_str+= dataSize;
			MPI_Recv(temp_str, dataSize, MPI_CHAR, i, 0, MPI_COMM_WORLD, &status);
		}

		for (int k = 0; k < Count; k++) {
			putc(str_file_t[k], output_file);
		}
		fclose(output_file);
		fopen_s(&output_file, "Ptestout.txt", "rb");
		fopen_s(&input_file, "test.txt", "rb");
		Testing(output_file, input_file, Count);

		fclose(output_file);
		fclose(input_file);

		delta_time_2 = time2 - time1;
		printf_s("\n Parallel Time: %f", delta_time_2);
		printf_s("\n Acceleration: %f", delta_time_1 / delta_time_2);
		printf_s("\n");
		free(temp_out_length);
		free(str_file);
		free(out_str);
		free(str_file_t);
	}
	
	if (ProcRank != 0) {
		double Ptime_1, Ptime_2;
		int c = 3*dataSize/2;
		char *str_file1 = (char *)malloc(dataSize * sizeof(char));
		char* out_str1 = (char*)malloc((c) * sizeof(char));
		char *str_file2 = (char *)malloc(dataSize * sizeof(char));
		for (int i = 0; i < c; i++)
			out_str1[i] = -1;

		MPI_Recv(str_file1, dataSize, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);

		Ptime_1 = MPI_Wtime();
		int length_out = 0;
		out_str1 = compress(str_file1, dataSize, out_str1, c , length_out);
		
		Ptime_2 = MPI_Wtime();

		MPI_Send(&length_out, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

		char* temp = out_str1;
		MPI_Send(temp, length_out, MPI_CHAR, 0, 0, MPI_COMM_WORLD);

		double Pdelta = Ptime_2 - Ptime_1;
		printf("ProcRank = %d , time = %f", ProcRank, Pdelta);
		printf("\n");

		str_file2 = expand(out_str1, str_file2, length_out, dataSize);
		
		MPI_Send(str_file2, dataSize, MPI_CHAR, 0, 0, MPI_COMM_WORLD);

		free(str_file1);
		free(out_str1);
		free(str_file2);
	}

	MPI_Finalize();
	return 0;
}