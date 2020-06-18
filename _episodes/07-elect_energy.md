---
title: "Calculating Electrostatic Energy"
teaching: 20
exercises: 10
questions:
- "How do we handle irregular tasks?"
objectives:
- "Learn about the schedule() clause"
keypoints:
- "Different loop scheduling may compensate for unbalanced loop iterations"
---

Calculating total electrostatic potential energy of a set of charges is a common problem in molecular simulations. To compute it we need to sum interactions between all pairs of charges:

$$ E = \frac{charge_i*charge_j}{distance_{i,j}}$$

Start with the following boilerplate:
~~~
/* --- File elect_energy.c --- */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int main(int argc, char **argv) {
	struct timespec ts_start, ts_end;
	int size = 20;
	int n_charges = size*size*size;
	float scale=0.5;
	float *charge, *x, *y, *z;
	float **M;
	int i,j,k;
	float time_total;

	charge=malloc(n_charges*sizeof(float));
	x=malloc(n_charges*sizeof(float));
	y=malloc(n_charges*sizeof(float));
	z=malloc(n_charges*sizeof(float));
	M=(float**)malloc(n_charges*sizeof(float*));
	for (i=0;i<n_charges;i++)
		M[i]=malloc(n_charges*sizeof(float));

/* initialize x,y,z coordinates and charges */
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


	// Calculate electrostatic energy: sum of charge[i]*charge[j]/dist[i,j] */
	float dx, dy, dz, dist;
	double Energy=0.0f;

/* Loop calculating  Energy = charge[i]*charge[j]/dist[i,j] */

	clock_gettime(CLOCK_MONOTONIC, &ts_end);
	time_total = (ts_end.tv_sec - ts_start.tv_sec)*1000000000 + (ts_end.tv_nsec - ts_start.tv_nsec);
	printf("\nTotal time is %f ms, Energy is %f\n", time_total/1000000, Energy);
}
~~~
{: .source}

This code generates a set of charges placed at the nodes of the cubic grid. The grid spacing is defined by the *scale* variable.

1. Write the loop computing sum of all pairwise electrostatic interactions. Recollect that because $E_{ij}=E_{ji}$ we need to compute only half of all interactions.
2. First, compile and run the program without OpenMP. Note how long it took to run. A millisecond is not enough to get good performance measurements.
3. Next, increase the `size` to 30 and recompile. Check the run time. Try with the size = 40.

Now comes the parallelization.

> ## Parallelize the energy code
> 1. Decide what variable or variables should be made private, and then compile and test the code.
> 2. Run on few different numbers of CPUs. How does the performance scale?
>
> > ## Solution
> > Parallelized version: elect_energy_omp.c
> {: .solution}
{: .challenge}

## The schedule() clause

The `schedule()`, clause allows us to specify how loop iterations are divided up among the threads.

- The default is *static* scheduling, in which all iterations are allocated to threads before they execute any loop iterations.

- In *dynamic* scheduling, only some of the iterations are allocated to threads at the beginning of the loop's execution. Threads that complete their iterations are then eligible to get additional work. The allocation process continues until all the iterations have been distributed to threads.

- Static scheduling has low overhead but
may be badly balanced; dynamic scheduling has higher overhead.

- Both scheduling types also take a *chunk size*; larger chunks mean less overhead and greater memory locality, smaller chunks may mean finer load balancing.

Bad load balancing might be what's causing this code not to parallelize very well. As we are computing triangular part of the interaction matrix *static* scheduling with the default *chunk size* will lead to uneven load.

Let's add a `schedule(dynamic)` or `schedule(static,100)` clause and see what happens.

> ## Play with the schedule() clause
>
> Try different `schedule()` clauses and tabulate the run times with different thread numbers. What seems to work best for this problem?
>
> Does it change much if you grow the problem size? That is, if you make `size` bigger?
>
> There's a third option, `guided`, which starts with large chunks and gradually decreases the chunk size as it works through the iterations.
> Try it out too, if you like. With `schedule(guided,<chunk>)`, the chunk parameter is the smallest chunk size it will try.
{: .challenge}
