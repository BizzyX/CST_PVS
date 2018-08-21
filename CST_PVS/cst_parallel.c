#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <time.h>

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#define BUFF_SIZE 1000

void testGetFile();
char randomStringNumber();


/*
	TODO: 
			- Funktion um einen Random String der 6 Zeichen lang ist und nur Zahlen im Bereich 0-6 enthält
			- Es wird übergeben beim Programmaufruf, wie viele Strings eingelesen werden sollen
			- Verbosity übergeben
			- Printfunktion
*/


int main(int argc, char* argv[])
{
	

	//Initialize the MPI environment
	MPI_Init(NULL, NULL);

	char buf[BUFF_SIZE];
	int numberArray[BUFF_SIZE];
	int i;


	//----- MPI FUNCTIONS --------
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

	//Testvariables
	char str1[] = "123456";
	char str2[] = "122456";
	int Lenght = 6;

	//randomStringNumber(Lenght);


	hammingDistance(str1,str2);

	system("pause");
	return 0;

}

void testGetFile() {
	
	// open file
	FILE *fp = fopen("strings.txt", "r");
	size_t len = 255;
	
	int arraySize = 0;
	int arrayLenght = 0;
	void** stringArray;
	
	// need malloc memory for line, if not, segmentation fault error will occurred.
	char *line = malloc(sizeof(char) * len);
	
	// check if file exist (and you can open it) or not
	if (fp == NULL) {
		printf("can't open file strings.txt!");
		return;
	}

	//read the first line - write it into the array size
	if (fgets(line, len, fp) != NULL) {
	
		arraySize = atoi(line);
		printf("Size: %s\n", line);
		
	}

	//read the second line - write it into the arrayLenght
	if (fgets(line, len, fp) != NULL) {

		arrayLenght = atoi(line);
		printf("Lenght: %s\n", line);

	}
	printf("Content of Array: \n\n");
	while (fgets(line, len, fp) != NULL) {
		int count = 0;
		stringArray = malloc(arraySize * sizeof * stringArray);
		stringArray[count] = line; 
		printf("%s\n", stringArray[count]);
		count++;
	}
	printf("\n");
	
	free(line);
}

int hammingDistance(char *str1, char *str2)
{
	int i = 0, count = 0;
	while (str1[i] != NULL)
	{
		
		if (str1[i] != str2[i])
		{
			count++;
		}
		i++;
	}
	printf("the hamming distance is: %d \n", count);
	
	return count;
	
}


/*
char randomStringNumber(int *Lenght) 
{
	int numberLenght = Lenght;
	printf("numberLenght:%d\n", numberLenght);
	srand(time(NULL));
	
	//malloc for string lenght
	char *stringLenght = malloc(sizeof(char) * numberLenght);
	//array for random string
	void** stringArray;
	stringArray = malloc(numberLenght * sizeof *stringArray);
	printf("stringArray: %d", stringArray);

	
	

	for (int i = 1; i <= numberLenght; i++)
	{
		
		int randomNumber = (rand() % numberLenght + 1);
		stringArray[i] = randomNumber + '0';
		printf("%d\n", stringArray[i]);

	}
	printf("Stringnumber: %d\n", stringArray);
}
*/