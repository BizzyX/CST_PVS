#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <mpi.h>
#include <time.h>

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#define BUFF_SIZE 1000

typedef struct stringArray_
{
	void** data;
	int stringSize;
	int stringLength;

} stringArray;

stringArray testGetFile();
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
	//_Init(NULL, NULL);

	char buf[BUFF_SIZE];
	int numberArray[BUFF_SIZE];
	int i;
	stringArray stringArray;

	/*
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
	*/

	stringArray = testGetFile();

	

	//Testvariables
	char str1[] = "123456";
	char str2[] = "122456";
	int Lenght = 6;

	//randomStringNumber(Lenght);


	hammingDistance(str1,str2);


	

	printf("%d \n", stringArray.stringSize);
	printf("%d \n", stringArray.stringLength);
	system("pause");
	for (int i = 0; i < stringArray.stringSize; i++) {
	
		printf("%s \n", stringArray.data[i]);

	}
	

	

	system("pause");
	return 0;

}

stringArray testGetFile() {
	
	// open file
	FILE *fp = fopen("strings.txt", "r");
	size_t len = 255;
	stringArray stringArrayTemp;

	static int arraySize = 0;
	static int arrayLength = 0;

	// need malloc memory for line, if not, segmentation fault error will occurred.
	char(*line)[] = malloc(sizeof(char) * len);

	// check if file exist (and you can open it) or not
	if (fp == NULL) {
		printf("can't open file strings.txt!");
		return ;
	}

	//read the first line - write it into the array size
	if (fgets(line, len, fp) != NULL) {

		stringArrayTemp.stringSize = atoi(line);
		printf("Size: %s\n", line);

	}

	//stringArray = malloc(arraySize * sizeof * stringArray);
	//read the second line - write it into the arrayLenght
	if (fgets(line, len, fp) != NULL) {

		stringArrayTemp.stringLength = atoi(line);
		printf("Lenght: %s\n", line);

	}
	printf("Content of Array: \n\n");
	int count = 0;
	stringArrayTemp.data = malloc(arraySize * sizeof * stringArrayTemp.data);
	while (fgets(line, len, fp) != NULL) {
		printf("%s", line);
		char(*persistString)[] = malloc(sizeof(char) * len);
		memcpy(persistString, line, sizeof(line));
		
		stringArrayTemp.data[count] = persistString;
		printf("%s\n", stringArrayTemp.data[0]);
		count = count + 1;
	}
	printf("\n");

	int *sizeLength = malloc(2 * sizeof * sizeLength);
	sizeLength[0] = stringArrayTemp.stringSize;
	sizeLength[1] = stringArrayTemp.stringLength;

	//free(line);
	return stringArrayTemp;
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