#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#define BUFF_SIZE 1000

void testGetFile();


int main(int argc, char* argv[])
{
	

	//Initialize the MPI environment
	MPI_Init(NULL, NULL);

	//read from .txt file 'strings.txt'
	//FILE *ptr_strings = (fopen_s(&ptr_strings, "strings.txt", "r"));
	char buf[BUFF_SIZE];
	int numberArray[BUFF_SIZE];
	int i;

	/*
	//read unsuccesful
	if (!ptr_strings)
	{
		printf("File Cannot Be Opened!\n");
		return 1;
	}
	//read succesful
	if (ptr_strings)
	{

		printf("File was succesfully openend!\n");
		
		//EXCEPTION
		/*
		for ( i = 0; i < BUFF_SIZE; i++)
		{
			fscanf(ptr_strings, "%1d", &numberArray[i]);
		}
		for ( i = 0; i < BUFF_SIZE; i++)
		{
			printf("Number is: %d\n\n", numberArray[i]);
		}
		
	
}
*/

	//Get the number of processes
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	//Get the rank of the process
	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	//Get the name of the processor
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int name_len;
	MPI_Get_processor_name(processor_name, &name_len);

	//Finalize the MPI Environment
	MPI_Finalize();
	testGetFile();

	system("pause");
	return 0;

}

void testGetFile() {
	// open file
	FILE *fp = fopen("strings.txt", "r");
	size_t len = 255;
	int arraySize = 0;
	void** stringArray;
	// need malloc memory for line, if not, segmentation fault error will occurred.
	char *line = malloc(sizeof(char) * len);
	// check if file exist (and you can open it) or not
	if (fp == NULL) {
		printf("can't open file input1.txt!");
		return;
	}

	if (fgets(line, len, fp) != NULL) {
	
		arraySize = atoi(line);
		printf("%s\n", line);
		//printf("test");
	
	}

	while (fgets(line, len, fp) != NULL) {
		int count = 0;
		//printf("%d\n", arraySize);
		stringArray = malloc(arraySize * sizeof * stringArray);
		stringArray[count] = line; 
		//printf("%d\n", sizeof(void*));
		printf("%s\n", stringArray[count]);
		count++;
	}
	free(line);
}