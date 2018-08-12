#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#define BUFF_SIZE 1000


int main(int argc, char* argv[])
{
	

	//Initialize the MPI environment
	MPI_Init(NULL, NULL);

	//read from .txt file 'strings.txt'
	FILE *ptr_strings = (fopen_s(&ptr_strings, "strings.txt", "r"));
	char buf[BUFF_SIZE];
	int numberArray[BUFF_SIZE];
	int i;

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
		*/
	}

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

	system("pause");
	return 0;

}