#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <time.h>
#include <math.h>

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

/*

		How to run this Program on MPI: 
		run in the cmd line:	mpiexec -n [numberOfProcessors] CST_PVS.exe [StringsToRead][Verbosity]
		Example:				mpiexec -n 4 CST_PVS.exe 7 3

		Verbosity: 
			0 = Program does not print any information on screen
			1 = Program will print the results
			2 = Program will print the time it took to process
			3 = Program will print the time + results

*/


//struct to store the data from the txt file
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
	clock_t begin, end;							//to stop the time
	int stringsToRead = atoi(argv[1]);			//how many strings to read from the txt file
	int verbosity = atoi(argv[2]);				//user can choose from a verbosity from 0-3
	int myrank, nprocs;							//MPI Variables
	int namelen;								//Processor-Name
	char name[MPI_MAX_PROCESSOR_NAME];	

	
	int i;										//counter
	stringArray stringArray;					//member-varible from struct
	int bestDistanceFound = INT_MAX;			//startvalue for bestdistancefound - any distance that is found should be lass than INT_MAX

	
	stringArray = testGetFile(stringsToRead);	//read all the info from the input textfile into the struct stringArray

	//malloc memory for the best string found on each process
	char *result = malloc(stringArray.stringLength + 1 * sizeof(char));
	double tempEnd = (double)stringArray.stringLength;
	double hex = (double)16;
	unsigned long long startRegion = 0, endRegion = pow(hex, tempEnd), regionSizePerProc = 0;


	//----- MPI FUNCTIONS --------
	/* how many processes we have */
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

	/* what is my rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	/* what is my hostname */
	MPI_Get_processor_name(name, &namelen);

	//we are on the root process now
	if (myrank == 0)
	{
		//malloc memory for the stringlenght + 1 (distance) * number of processors
		char *bestResults = malloc(sizeof(char) * (stringArray.stringLength + 1) * nprocs);

		//region that every processor have to go through
		regionSizePerProc = endRegion / (unsigned long long)nprocs;

		unsigned long long regionSizePerProcCeil = ceil(regionSizePerProc);
		//start the time
		begin = clock();
		//go through the region that processor 0 is responsible for
		result = generateString((unsigned long long)0, regionSizePerProcCeil, stringArray, &bestDistanceFound);

		char *buffer = malloc((stringArray.stringLength + 2) * sizeof(char));
		//build new string with the best string found + distance
		snprintf(buffer, sizeof(char)*stringArray.stringLength + 2, "%s%d", result, bestDistanceFound);
		memcpy(bestResults, buffer, stringArray.stringLength + 2);
	
		//receive from all other processes
		for (i = 0; i < nprocs; i++)
		{
			//on all other processes than 0
			if (i != myrank)
			{
				MPI_Recv(bestResults + (i) * (stringArray.stringLength + 1), sizeof(char) * (stringArray.stringLength + 1), MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);

			}


		}

		//Initialize for the best distance found and the position where the string is
		int bestDistanceFinal = INT_MAX, positionOfBestString = 0;
		//PositionOfBestString will store the best Distance that is found
		for (int k = 1; k <= nprocs; k++) {

			int distanceTemp = *(bestResults + stringArray.stringLength * k + (k - 1)) - 48;
			if (distanceTemp < bestDistanceFinal) {

				bestDistanceFinal = distanceTemp;
				positionOfBestString = k;

			}

		}

		//load best string overall from all processes
		char *bestStringFinal = malloc(sizeof(char) * stringArray.stringLength + 1);
		memcpy(bestStringFinal, (bestResults + stringArray.stringLength * (positionOfBestString - 1) + (positionOfBestString - 1)), stringArray.stringLength);
		bestStringFinal[stringArray.stringLength] = '\0';
		//stop the time
		end = clock();
		float t_ges = end - begin;
		t_ges = t_ges / CLOCKS_PER_SEC;
		
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

		regionSizePerProc = endRegion / (unsigned long long)nprocs;

		unsigned long long regionSizePerProcCeil = ceil(regionSizePerProc);
		unsigned long long regionSizePerProcFloor = floor(regionSizePerProc);
		startRegion = regionSizePerProcFloor * (unsigned long long)myrank;
		result = generateString(startRegion, startRegion + regionSizePerProcCeil, stringArray, &bestDistanceFound);
		char *buffer = malloc(stringArray.stringLength + 2 * sizeof(char));
		snprintf(buffer, sizeof(char)*stringArray.stringLength + 2, "%s%d", result, bestDistanceFound);
		
		//send best string + distance
		MPI_Send(buffer, stringArray.stringLength + 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);

	}
	//Finalize the MPI-Environment
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


	// need malloc memory for line to read from .txt file
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
	//read all the given strings into stringArrayTemp.data
	while (fgets(line, len, fp) != NULL) {

		
		char *persistString = malloc(sizeof(char) * stringArrayTemp.stringLength + 1);
		memcpy(persistString, line, stringArrayTemp.stringLength + 1 * sizeof(char));
		if(persistString[stringArrayTemp.stringLength] == '\n')
		persistString[stringArrayTemp.stringLength] = '\0';
		stringArrayTemp.data[count] = persistString;

		count = count + 1;
	}
	free(line);

	int *sizeLength = malloc(2 * sizeof * sizeLength);
	sizeLength[0] = stringArrayTemp.stringSize;
	sizeLength[1] = stringArrayTemp.stringLength;


	return stringArrayTemp;
}

//will return count which stores the distance between the two input strings
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

//generates hex-strings from the startregion to the endregion and finds the best string in the given region
char* generateString(unsigned long long start, unsigned long long end, stringArray stringArrayCompare, int *bestDistanceFound)
{
	int distance = -1, bestDistanceLoop = -1;
	unsigned long long myHex = 0x000000;
	myHex += start;
	char *tempLength = malloc(stringArrayCompare.stringLength * sizeof * tempLength);

	char buffer[1024];
	sprintf(tempLength, "%d", stringArrayCompare.stringLength);
	snprintf(buffer, sizeof(buffer), "%%0%dllx", stringArrayCompare.stringLength);

	char *stringToTest = malloc(stringArrayCompare.stringLength + 1 * sizeof(char));
	char *bestStringFound = malloc(stringArrayCompare.stringLength + 1 * sizeof(char));
	


	for (unsigned long long i = start; i < end; i++) {
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

//Print the best results with the distance and the arraydata
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

