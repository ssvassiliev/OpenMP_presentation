#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int main(int argc, char **argv) {
	struct timespec ts_start, ts_end;
	int size = 30;
	int npoints = size*size*size;
	float scale=0.5;
	float *charge, *x, *y, *z;
	float **M;
	int i,j,k;
	float time_total;


	charge=malloc(npoints*sizeof(float));
	x=malloc(npoints*sizeof(float));
	y=malloc(npoints*sizeof(float));
	z=malloc(npoints*sizeof(float));
	M=(float**)malloc(npoints*sizeof(float*));
	for (i=0;i<npoints;i++)
		M[i]=malloc(npoints*sizeof(float));


	int n=0;
	for (i=0; i<size; i++)
		for (j=0; j<size; j++) 
			for (k=0; k<size; k++) {
				x[n]=i*scale;
				y[n]=j*scale;
				z[n]=k*scale;
				charge[n]=0.33;
				n++;
			}
	clock_gettime(CLOCK_MONOTONIC, &ts_start);


	// Calculate electrostatic enegry
	float dx, dy, dz, dist;
	double Energy=0.0f;
#pragma omp parallel for private(j,dx,dy,dz,dist,n) reduction(+:Energy) schedule(static,50)
	for (i = 0; i < npoints; i++) {
		for (j = i+1; j < npoints; j++) {
			dx = x[i]-x[j];
			dy = y[i]-y[j];
			dz = z[i]-z[j];
			dist=sqrt(dx*dx + dy*dy + dz*dz);
			Energy += charge[i]*charge[j]/dist;
		}
	}

	clock_gettime(CLOCK_MONOTONIC, &ts_end);
	time_total = (ts_end.tv_sec - ts_start.tv_sec)*1000000000 + (ts_end.tv_nsec - ts_start.tv_nsec);
	printf("\nTotal time is %f ms, Energy is %f\n", time_total/1000000, Energy);
}

