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

int calcHistogram (void *arg);
float binSize;

int main(void) {

	float rawData[1000000];

	FILE *fileIn = fopen("numbers.bin", "rb");
	if (fileIn == NULL){
		printf("File not opened\n");
		return 0;
	}

	int cnt = 1000000;
	int splitCnt = cnt/2;
	int resultCode;

	resultCode = fread(rawData, sizeof(rawData[0]), sizeof(rawData)/sizeof(rawData[0]), fileIn);

	fclose(fileIn);

	// make two smaller arrays by splitting the rawData in half
	float *rawDataHalf1 = malloc(splitCnt * sizeof(float));
	float *rawDataHalf2 = malloc(splitCnt * sizeof(float));

	memcpy(rawDataHalf1, rawData, splitCnt * sizeof(float));
	memcpy(rawDataHalf2, rawData + splitCnt, splitCnt * sizeof(float));

	// create a thread for each half.
	pthread_t id1;
	struct histogram_arg_struct args1;
	args1.histogramVals = malloc(NUMBINS * sizeof(int));
	args1.rawData = rawDataHalf1;
	args1.numDataPoints = splitCnt;
	args1.numBins = NUMBINS;
	pthread_create(&id1, NULL, calcHistogram, &args1);


	pthread_t id2;
	struct histogram_arg_struct args2;
	args2.histogramVals = malloc(NUMBINS * sizeof(int));
	args2.rawData = rawDataHalf2;
	args2.numDataPoints = splitCnt;
	args2.numBins = NUMBINS;
	pthread_create(&id2, NULL, calcHistogram, &args2);

	// joining threads
	pthread_join(id1, NULL);
	int *histogramVals1 = args1.histogramVals;
	pthread_join(id2, NULL);
	int *histogramVals2 = args2.histogramVals;

	// add the two threads' histograms together
	int *newHistogramVals = malloc(NUMBINS * sizeof(int));
	for (int c = 0; c < NUMBINS; c++) {
		int newVal = histogramVals1[c] + histogramVals2[c];
		newHistogramVals[c] = newVal;
	}

	//	// print values of the new Histogram
	for (int p = 0; p < NUMBINS; p++){
		printf("bin: %d  binStart: %f   binEnd:  %f  Histogram Value: %d\n", p, binSize*(float)p, binSize*(float)(p+1), newHistogramVals[p]);
	}

	return EXIT_SUCCESS;
}



/*-------------------------------------------------------------------
 * Function:    calcHistogram
 * Purpose:     Calculate the histogram values of a local histogram struct
 * In args:     void * arg whose value represents the histogram struct to cast from
 * Outputs:     Histogram of values stored in global histogramVals
				binSize in local histogram struct
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

