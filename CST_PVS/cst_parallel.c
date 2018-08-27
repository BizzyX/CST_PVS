#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <time.h>
#include <math.h>

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

typedef struct stringArray_
{
	void** data;
	int stringSize;
	int stringLength;

} stringArray;

//initialize the functions
stringArray testGetFile();
char randomStringNumber();
char *generateString();
void printResults();
//void printTimeToTxt();


/*
			Original-Text-File
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

	//Variables
	clock_t begin, end;
	int stringsToRead = atoi(argv[1]);
	int verbosity = atoi(argv[2]);
	int myrank, nprocs;
	int namelen;
	char name[MPI_MAX_PROCESSOR_NAME];

	//counter
	int i;
	stringArray stringArray;
	int bestDistanceFound = INT_MAX;

	//read all the info from the input textfile into the struct stringArray
	stringArray = testGetFile(stringsToRead);

	//malloc memory for the best string found
	char *result = malloc(stringArray.stringLength + 1 * sizeof(char));
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
		//malloc memory for the stringlenght + 1 (distance) * number of processors
		char *bestResults = malloc(sizeof(char) * (stringArray.stringLength + 1) * nprocs);

		//region that every processor have to go through
		regionSizePerProc = endRegion / nprocs;

		double regionSizePerProcCeil = ceil(regionSizePerProc);
		//start the time
		begin = clock();
		//go through the region that processor 0 is responsible for
		result = generateString((double)0, regionSizePerProcCeil, stringArray, &bestDistanceFound);

		char *buffer = malloc((stringArray.stringLength + 2) * sizeof(char));
		snprintf(buffer, sizeof(char)*stringArray.stringLength + 2, "%s%d", result, bestDistanceFound);
		memcpy(bestResults, buffer, stringArray.stringLength + 2);
		//memcpy(bestResults + (nprocs * (stringArray.stringLength + 2)), "\0", sizeof(char));



		//broadcast to all other processes
		for (i = 0; i < nprocs; i++)
		{
			if (i != myrank)
			{
				MPI_Recv(bestResults + (i) * (stringArray.stringLength + 1), sizeof(char) * (stringArray.stringLength + 1), MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);

			}


		}

		//printf("%s\n\n", bestResults);

		int bestDistanceFinal = INT_MAX, positionOfBestString = 0;

		for (int k = 1; k <= nprocs; k++) {

			int distanceTemp = *(bestResults + stringArray.stringLength * k + (k - 1)) - 48;
			if (distanceTemp < bestDistanceFinal) {

				bestDistanceFinal = distanceTemp;
				positionOfBestString = k;

			}

		}


		char *bestStringFinal = malloc(sizeof(char) * stringArray.stringLength + 1);
		memcpy(bestStringFinal, (bestResults + stringArray.stringLength * (positionOfBestString - 1) + (positionOfBestString - 1)), stringArray.stringLength);
		bestStringFinal[stringArray.stringLength] = '\0';

		end = clock();

		float t_ges = end - begin;
		t_ges = t_ges / CLOCKS_PER_SEC;
		//printTimeToTxt(t_ges, nprocs, stringArray.stringLength);
		//TODO: write result into a text file
		//		with number of procs and string lenght

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
	}
	else
	{

		regionSizePerProc = endRegion / nprocs;

		double regionSizePerProcCeil = ceil(regionSizePerProc);
		double regionSizePerProcFloor = floor(regionSizePerProc);
		startRegion = regionSizePerProcFloor * myrank;
		result = generateString(startRegion, startRegion + regionSizePerProcCeil, stringArray, &bestDistanceFound);
		char *buffer = malloc(stringArray.stringLength + 2 * sizeof(char));
		snprintf(buffer, sizeof(char)*stringArray.stringLength + 2, "%s%d", result, bestDistanceFound);
		

		//result[stringArray.stringSize + 1] = bestDistanceFound +'0';
		//printf("%s\n", result);
		//printf("%s\n", buffer);
		//printf("Start: %f Step: %f END: %f BestResult: %s\n\n" , startRegion, regionSizePerProc, startRegion + regionSizePerProcCeil,buffer);

		MPI_Send(buffer, stringArray.stringLength + 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);

	}
	MPI_Finalize();

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
		return;
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


	}


	//read the second line - write it into the arrayLenght
	if (fgets(line, len, fp) != NULL) {

		stringArrayTemp.stringLength = atoi(line);


	}
	free(line);
	line = malloc(stringArrayTemp.stringLength + 1 * sizeof * stringArrayTemp.data);

	int count = 0;
	stringArrayTemp.data = malloc(stringArrayTemp.stringSize * sizeof * stringArrayTemp.data +1);
	while (fgets(line, len, fp) != NULL) {

		//where buff is your char array fgets is using
		char *persistString = malloc(sizeof(char) * stringArrayTemp.stringLength + 1);
		memcpy(persistString, line, stringArrayTemp.stringLength + 1 * sizeof(char));
		if(persistString[stringArrayTemp.stringLength] == '\n')
		persistString[stringArrayTemp.stringLength] = '\0';
		stringArrayTemp.data[count] = persistString;

		//printf("StringData: %s\n", stringArrayTemp.data[count]);

		count = count + 1;
	}
	free(line);

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
	unsigned long long myHex = 0x000000;
	myHex += start;
	char *tempLength = malloc(stringArrayCompare.stringLength * sizeof * tempLength);

	char buffer[1024];
	sprintf(tempLength, "%d", stringArrayCompare.stringLength);
	snprintf(buffer, sizeof(buffer), "%%0%dx", stringArrayCompare.stringLength);

	char *stringToTest = malloc(stringArrayCompare.stringLength + 1 * sizeof(char));
	char *bestStringFound = malloc(stringArrayCompare.stringLength + 1 * sizeof(char));
	


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
			memcpy(bestStringFound, stringToTest, stringArrayCompare.stringLength + 1 * sizeof(char));
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
		printf("%s        %d", stringArrayCompare.data[i], distanceTemp);
		printf("\n");
	}
	printf("============================\n\n");

}

