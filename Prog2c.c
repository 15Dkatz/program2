/*
 ============================================================================
 Name        : prog2.c
 Author      :
 Version     :
 Copyright   :
 Description : Serial version of calculate histogram program
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <pthread.h>
#include <string.h>

#define NUMBINS 1000

struct histogram_arg_struct {
	int *histogramVals; /* local histogram for each thread */
	float *rawData;
	int numDataPoints;
	int numBins;
};

//int *histogramVals;    /*global histogram array for all threads*/
float binSize;		   /*bin size calculated from min and max of array values*/

int calcHistogram (void *arg);

int main(void) {

	float rawData[1000000];

//	histogramVals =  malloc(NUMBINS * sizeof(int));

	FILE *fileIn = fopen("numbers.bin", "rb");
	if (fileIn == NULL){
		printf("File not opened\n");
		return 0;
	}

	int cnt = 1000000;
	int resultCode;

	resultCode = fread(rawData, sizeof(rawData[0]), sizeof(rawData)/sizeof(rawData[0]), fileIn);

	fclose(fileIn);

	// calcHistogram (rawData, cnt, NUMBINS);

	// make two smaller arrays by splitting the rawData in half
	float *rawDataHalf1 = malloc(cnt/2 * sizeof(float));
	float *rawDataHalf2 = malloc(cnt/2 * sizeof(float));

	memcpy(rawDataHalf1, rawData, cnt/2 * sizeof(float));
	memcpy(rawDataHalf2, rawData + cnt/2, cnt/2 * sizeof(float));

	// create a thread for each half.
	pthread_t id1;
	struct histogram_arg_struct args1;
	args1.histogramVals = malloc(NUMBINS * sizeof(int));
	args1.rawData = rawDataHalf1;
	args1.numDataPoints = cnt/2;
	args1.numBins = NUMBINS;
	pthread_create(&id1, NULL, calcHistogram, &args1);

	for (int cnt1 = 0; cnt1 < NUMBINS; cnt1++){
		//printf("bin: %d  binStart: %f   binEnd:  %f  Histogram Value: %d\n", cnt1, binSize*(float)cnt1, binSize*(float)(cnt1+1), histogramVals[cnt1]);
	}

	return EXIT_SUCCESS;
}



/*-------------------------------------------------------------------
 * Function:    calcHistogram
 * Purpose:     Calculate Histogram result is in a global variable histogramVals.
 * In args:     data:  values to be histogrammed.
 *              numDataPoints:  Number of data points to be histogrammed.
 *              numBins:  Number of bins in output histogram
 * Outputs:     Histogram of values stored in global histogramVals
				binSize in global variable.
 *
 */

int calcHistogram (void *arg) {
	struct histogram_arg_struct *arg_struct = (struct histogram_arg_struct *) arg;
	float *data = arg_struct->rawData;;
	int *histogramVals = arg_struct->histogramVals;
	int numDataPoints = arg_struct-> numDataPoints;
	int numBins = arg_struct->numBins;

	float maxVal = FLT_MIN;
	float minVal = FLT_MAX;

	for(int cnt = 0; cnt < numDataPoints; cnt++ ) {
		if ( data[cnt] > maxVal ) maxVal = data[cnt];
		if ( data[cnt] < minVal ) minVal = data[cnt];
	}

	binSize = (maxVal-minVal)/(float)numBins;
	printf("minVal: %f  maxVal: %f  binSize: %f\n", minVal, maxVal, binSize);

	for (int i = 0; i < numDataPoints; i++){
		int binNum = (int)((data[i] - minVal) / binSize);
		histogramVals[binNum]++;
	}
	return 0;
}

