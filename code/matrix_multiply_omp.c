/* --- File integrate_sin_omp.c --- */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

int main(int argc, char **argv) {
	struct timespec ts_start, ts_end;
	int size = 1e4;
	int **a, *c, total;
	int i, j;
	float time_total;

	/* Allocate memory */
	c=malloc(size*sizeof(int));
	a=(int**)malloc(size*sizeof(int*));
	for(i=0;i<size;i++)
		a[i]=malloc(size*sizeof(int));

	/* Set the matrix values to 1 */
	for (i=0; i<size; i++) {
		for (j=0; j<size; j++) {
			a[i][j] = 1;
		}
	}

	/* Zero the accumulator */
	for (i=0; i<size; i++) {
		c[i] = 0;
	}
	total = 0;

	clock_gettime(CLOCK_MONOTONIC, &ts_start);

#pragma omp parallel for
	for (i = 0; i<size; i++) {
		for (j=0; j<size; j++) {
			c[i] += a[i][j];
		}
	}

	for (i=0; i<size; i++) {
		total += c[i];
	}

	clock_gettime(CLOCK_MONOTONIC, &ts_end);
	time_total = (ts_end.tv_sec - ts_start.tv_sec)*1e9 + \
		     (ts_end.tv_nsec - ts_start.tv_nsec);
	printf("Total is %d, time is %f ms\n", total, time_total/1e6);
}
