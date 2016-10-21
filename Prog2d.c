/*
 ============================================================================
 Name        : prog2d.c
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <float.h>
#include <pthread.h>

#define NUMBINS 1000

float binSize;		   /*bin size calculated from min and max of array values*/

int* calcHistogram (float* data, int numDataPoints, int numBins);

int main(void) {

	float rawData[1000000];

	FILE *fileIn = fopen("numbers.bin", "rb");
	if (fileIn == NULL){
		printf("File not opened\n");
		return 0;
	}

	int cnt = 1000000;
	int splitCnt = cnt/8;
	int resultCode;

	resultCode = fread(rawData, sizeof(rawData[0]), sizeof(rawData)/sizeof(rawData[0]), fileIn);

	fclose(fileIn);

	// creating 8 local arrays
	int* histogram0 = calcHistogram(rawData, splitCnt, NUMBINS);
	int* histogram1 = calcHistogram(rawData, splitCnt, NUMBINS);
	int* histogram2 = calcHistogram(rawData, splitCnt, NUMBINS);
	int* histogram3 = calcHistogram(rawData, splitCnt, NUMBINS);
	int* histogram4 = calcHistogram(rawData, splitCnt, NUMBINS);
	int* histogram5 = calcHistogram(rawData, splitCnt, NUMBINS);
	int* histogram6 = calcHistogram(rawData, splitCnt, NUMBINS);
	int* histogram7 = calcHistogram(rawData, splitCnt, NUMBINS);


	int *totalHistogram = malloc(NUMBINS);
	// adding the values of the 8 arrays together
	for (int c=0; c<NUMBINS; c++) {
		int newVal = histogram0[c] + histogram1[c] + histogram2[c] + histogram3[c] + histogram4[c] + histogram5[c] + histogram6[c] + histogram7[c];
		totalHistogram[c] = newVal;
	}

	// print the values in totalHistogram
	for (int p=0; p<NUMBINS; p++) {
		printf("Bin: %d Histogram Value: %d\n", p, totalHistogram[p]);
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

int* calcHistogram (float* data, int numDataPoints, int numBins) {
	int *histogramVals = malloc(numBins * sizeof(int));

	float maxVal = FLT_MIN;
	float minVal = FLT_MAX;

	for(int cnt = 0; cnt < numDataPoints; cnt++ ) {
		if ( data[cnt] > maxVal ) maxVal = data[cnt];
		if ( data[cnt] < minVal ) minVal = data[cnt];
	}

	binSize = (maxVal-minVal)/(float)numBins;
	// printf("minVal: %f  maxVal: %f  binSize: %f\n", minVal, maxVal, binSize);

	for (int i = 0; i < numDataPoints; i++){
		int binNum = (int)((data[i] - minVal) / binSize);
		histogramVals[binNum]++;
	}
	return histogramVals;
}
