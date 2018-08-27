#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <time.h>
#include <math.h>

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

//#define BUFF_SIZE 1000

typedef struct stringArray_
{
	void** data;
	int stringSize;
	int stringLength;

} stringArray;

stringArray testGetFile();
char randomStringNumber();
char *generateString();
void printResults();


/*
	TODO: 
			- Funktion um einen Random String der 6 Zeichen lang ist und nur Zahlen im Bereich 0-6 enthält
			- Es wird übergeben beim Programmaufruf, wie viele Strings eingelesen werden sollen
			- Verbosity übergeben
			- Printfunktion

			Values:
			6
			6
			234150
			510423
			023541
			342015
			105234
			451302

*/


int main(int argc, char **argv)
{
	//Initialize the MPI environment
	MPI_Init(&argc, &argv);

	clock_t begin, end;
	int stringsToRead = atoi(argv[1]);
	int verbosity = atoi(argv[2]);

	int myrank, nprocs;
	int namelen;
	char name[MPI_MAX_PROCESSOR_NAME];

	//int *sizeLength = malloc(2 * sizeof(int));
	

	//char buf[BUFF_SIZE];
	//int numberArray[BUFF_SIZE];
	int i;
	stringArray stringArray;
	int bestDistanceFound = INT_MAX;

	
	

	stringArray = testGetFile(stringsToRead);

	char *result = malloc(stringArray.stringLength * sizeof(char));
	double tempEnd = (double)stringArray.stringLength;
	double hex = (double)16;
	double startRegion = 0, endRegion = pow(hex, tempEnd), regionSizePerProc = 0;
	
	//----- MPI FUNCTIONS --------
	/* how many processes we have */
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

	/* what is my rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	/* what is my hostname */
	MPI_Get_processor_name(name, &namelen);

	// insert your code here(!)
	if (myrank == 0)
	{

		
		char *bestResults = malloc(sizeof(char) * (stringArray.stringLength+1) * nprocs);

		regionSizePerProc = endRegion / nprocs;
		//printf("Regio : %f EndRegio : %f\n", regionSizePerProc, endRegion);
		double regionSizePerProcCeil = ceil(regionSizePerProc);
		begin = clock();
		result = generateString((double)0, regionSizePerProcCeil, stringArray, &bestDistanceFound);

		//printf("%s\n\n", result);
		//printf("%d\n", bestDistanceFound);

		char *buffer = malloc((stringArray.stringLength + 1) * sizeof(char));
		snprintf(buffer, sizeof(buffer)+1, "%s%d", result, bestDistanceFound);
		//printf("%d\n\n", bestDistanceFound);
		//printf("%s\n", buffer);
		memcpy(bestResults, buffer, stringArray.stringLength+1);


		//broadcast to all other processes
		for (i = 0; i < nprocs; i++)
		{
			if (i != myrank)
			{
				MPI_Recv(bestResults + (i) * (stringArray.stringLength + 1), sizeof(char) * (stringArray.stringLength + 2), MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
				
			}
			
			
		}
		//printf("%s", bestResults);
		//printf("%s", bestResults);
		//printf("%s\n", bestResults);
		

		int bestDistanceFinal = INT_MAX, positionOfBestString = 0;

		for (int k = 1; k <= nprocs; k++) {
		
			int distanceTemp = *(bestResults + stringArray.stringLength * k + (k-1)) -48;
			//printf("%d\n\n", distanceTemp);
			//printf("%s", bestResults);
			if (distanceTemp < bestDistanceFinal) {
			
				bestDistanceFinal = distanceTemp;
				positionOfBestString = k;
			
			}
		
		}

		char *bestStringFinal = malloc(sizeof(char) * stringArray.stringLength);
		memcpy(bestStringFinal, (bestResults + stringArray.stringLength * (positionOfBestString-1) + (positionOfBestString-1)), stringArray.stringLength);
		
		end = clock();

		float t_ges = end - begin;
		t_ges = t_ges / CLOCKS_PER_SEC;

		//printf("%d",bestDistanceFinal);
		if (verbosity == 1) {

			printResults(bestDistanceFinal, bestStringFinal, stringArray);

		}
		else if (verbosity == 2) {

			printf("\nThe program took %f seconds to finish!\n", t_ges);

		}
		else if (verbosity == 3) {

			printf("\nThe program took %f seconds to finish!\n\n\n", t_ges);
			printResults(bestDistanceFinal, bestStringFinal, stringArray);

		}
		//printf("%s", bestStringFinal);
		//printf("Dist = %d K= %d", bestDistanceFinal,positionOfBestString);
		//printResults(bestDistanceFinal, bestStringFinal, stringArray);

	}
	else 
	{
		//char *test = "1234562";

		regionSizePerProc = endRegion / nprocs;
		
		double regionSizePerProcCeil = ceil(regionSizePerProc);
		double regionSizePerProcFloor = floor(regionSizePerProc);
		startRegion = regionSizePerProcFloor * myrank;
		//printf("Regio: %f EndRegio : %f\n", regionSizePerProc, startRegion);

		result = generateString(startRegion, startRegion + regionSizePerProcCeil, stringArray, &bestDistanceFound);
		char *buffer = malloc(stringArray.stringLength + 1 * sizeof(char));
		snprintf(buffer, sizeof(buffer)+1, "%s%d", result,bestDistanceFound);
		//printf("%s\n\n", buffer);
	
		MPI_Send(buffer, stringArray.stringLength + 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
		//printf("Here is my Size %d and Length %d", sizeLength[0], sizeLength[1]);
	
	}
	MPI_Finalize();
	

	//read .txt file
	


	


	//
	//begin = clock();
	//result = generateString(startRegion, endRegion, stringArray, &bestDistanceFound);
	//end = clock();

	//float t_ges = end - begin;
	//t_ges = t_ges/CLOCKS_PER_SEC;
	//
	//
	//
	//

	//if (verbosity == 1) {
	//
	//	printResults(bestDistanceFound, result, stringArray);
	//
	//}
	//else if (verbosity == 2) {
	//
	//	printf("The program took %f seconds to finish!\n", t_ges);

	//}
	//else if (verbosity == 3) {
	//
	//	printf("The program took %f seconds to finish!\n\n\n", t_ges);
	//	printResults(bestDistanceFound, result, stringArray);
	//
	//}
	//
	//system("pause");
	////Finalize the MPI Environment
	//
	return 0;

}

stringArray testGetFile(int stringsToRead) {
	
	// open file
	FILE *fp = fopen("strings.txt", "r");
	size_t len = 255;
	stringArray stringArrayTemp;

	static int arraySize = 0;
	static int arrayLength = 0;


	// need malloc memory for line, if not, segmentation fault error will occurred.
	char *line = malloc(sizeof(char) * len);

	// check if file exist (and you can open it) or not
	if (fp == NULL) {
		printf("can't open file strings.txt!");
		return ;
	}

	//read the first line - write it into the array size
	if (fgets(line, len, fp) != NULL) {

		stringArrayTemp.stringSize = atoi(line);
		if (stringsToRead > stringArrayTemp.stringSize) {
		
			printf("Could not use %d strings because only %d strings exists!\nUsing max possible number of strings available\n\n", stringsToRead, stringArrayTemp.stringSize);

		}
		else {
		
			stringArrayTemp.stringSize = stringsToRead;

		}
		//printf("Size: %s\n", line);

	}

	//stringArray = malloc(arraySize * sizeof * stringArray);
	//read the second line - write it into the arrayLenght
	if (fgets(line, len, fp) != NULL) {

		stringArrayTemp.stringLength = atoi(line);
		//printf("Lenght: %s\n", line);

	}
	free(line);
	line = malloc(stringArrayTemp.stringLength * sizeof * stringArrayTemp.data);
	//printf("Content of Array: \n\n");
	int count = 0;
	stringArrayTemp.data = malloc(stringArrayTemp.stringSize * sizeof * stringArrayTemp.data);
	while (fgets(line, len, fp) != NULL) {
		//printf("%s", line);
		int len = strlen(line); //where buff is your char array fgets is using
		if (line[len - 1] == '\n')
			line[len - 1] = '\0';
		char *persistString = malloc(sizeof(char) * stringArrayTemp.stringLength);
		memcpy(persistString, line, sizeof(line));
		stringArrayTemp.data[count] = persistString;
		//printf("%s\n", stringArrayTemp.data[0]);
		count = count + 1;
	}
	free(line);
	//printf("\n");

	int *sizeLength = malloc(2 * sizeof * sizeLength);
	sizeLength[0] = stringArrayTemp.stringSize;
	sizeLength[1] = stringArrayTemp.stringLength;

	
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
	return count;
}

char* generateString(double start, double end, stringArray stringArrayCompare, int *bestDistanceFound)
{
	int distance = -1, bestDistanceLoop = -1;
	int myHex = 0x0000;
	myHex += start;
	char *tempLength = malloc(stringArrayCompare.stringLength * sizeof * tempLength);

	char buffer[1024];
	sprintf(tempLength, "%d", stringArrayCompare.stringLength);
	snprintf(buffer, sizeof(buffer), "%%0%dx",stringArrayCompare.stringLength);

	char *stringToTest = malloc(stringArrayCompare.stringLength * sizeof(char));
	char *bestStringFound = malloc(stringArrayCompare.stringLength+1 * sizeof(char)); 

	memcpy(bestStringFound+stringArrayCompare.stringLength , "\0", sizeof(char));
	
	
	for (double i = start; i < end; i++) {
		sprintf(stringToTest, buffer, myHex);
		bestDistanceLoop = -1;
		for (int j = 0; j < stringArrayCompare.stringSize; j++) {
		
			distance = hammingDistance(stringToTest, stringArrayCompare.data[j]);
			if (distance > bestDistanceLoop) {
				bestDistanceLoop = distance;
			}
		}
		if (bestDistanceLoop < *bestDistanceFound) {
			*bestDistanceFound = bestDistanceLoop;
			memcpy(bestStringFound, stringToTest, stringArrayCompare.stringLength * sizeof(char));
		}
		myHex++;	
	}
	return bestStringFound;
}

void printResults(int distance, char *closestString, stringArray stringArrayCompare)
{
	int distanceTemp;
	printf("============================\n");
	printf("Closest string: \n");
	printf("Distance %d\n", distance);
	printf("New string  %s\n", closestString);
	printf("String        Distance\n");
	for (int i = 0; i < stringArrayCompare.stringSize; i++) {
		distanceTemp = hammingDistance(closestString, stringArrayCompare.data[i]);
		printf("%s        %d", stringArrayCompare.data[i] , distanceTemp);
		printf("\n");
	}
	printf("============================\n\n");

}

