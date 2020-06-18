---
title: "Tasks"
teaching: 20
exercises: 5
questions:
- "How to recurse in parallel"
objectives:
- "Use general parallel tasks"
keypoints:
- "OpenMP can manage general parallel tasks"
---

- The last technique we will look at is the task construct. Tasks is the most recent addition to omp constructs.

- Before tasks were introduced only loops with a known length at run time were allowed. And only finite number of parallel sections.

- The task construct addresses these issues by generating a pool of tasks that are then executed by all available threads.

~~~
#pragma omp parallel
#pragma omp single
{
...
#pragma omp task
{ ... }
...
#pragma omp task
{ ... }
...
}
~~~
{: .source}


- If you need to have results from the tasks before you can continue, you can use the `taskwait` directive to tell OpenMP to pause your program until the tasks are done.

> ## Finding the Smallest Factor
> Use tasks to find the smallest factor of a large number (using 4993*5393 as test case). Generate a task for each trial factor. Start with this code:
>
> ~~~
> /* File find_factor.c */
> #include <stdio.h>
> #include <stdlib.h>
> #include <time.h>
> int main()
> {
>   long N = 4993*5393;
>   long factor=0;
> #pragma omp parallel
> #pragma omp single
>   for (long f=2; f <= N; f++)
>   {
>     { // see if `f' is a factor
>       if (N%f == 0) { // found factor!
>         factor = f;
>       }
>     }
>     if (factor > 0)
>       break;
>   }
>   if (factor > 0)
>     printf("Found a factor: %li\n",factor);
> }
> ~~~
> {: .source}
> - Turn the factor finding block into a task.
> - Run your program a number of times. Does it find the wrong factor? Why? Try to fix this.
> - Once a factor has been found, you should stop generating tasks.
{: .challenge}

### Task Synchronization

In general it is impossible to say when a task will be executed. Consider the code:

~~~
x = f();
#pragma omp task
{ y1 = g1(x); }
#pragma omp task
{ y2 = g2(x); }
z = h(y1)+h(y2);
~~~
{: .source}

- The code is incorrect, `z = h(y1)+h(y2)` could be executed before y1 and y2 are computed.
- Wait until tasks are finished: `taskwait` directive.

Correct code:
~~~
x = f();
#pragma omp task
{ y1 = g1(x); }
#pragma omp task
{ y2 = g2(x); }
#pragma omp taskwait
z = h(y1)+h(y2);
~~~
{: .source}

### Task Dependencies

Consider the following code:
~~~
#pragma omp task
x = f()
#pragma omp task
y = g(x)
~~~~
{: .source}

The second task can be executed before the first, possibly leading to an incorrect result. This is corrected by specifying task dependency:

~~~
#pragma omp task depend(out:x)
x = f()
#pragma omp task depend(in:x)
y = g(x)
~~~
{: .source}

This example illustrates Read after Write (RaW) dependency: the first task writes x which is then read by the second task.

Since either task can read or write a variable, there are four types of dependencies:

1. [RaW (Read after Write)]
2. [WaR (Write after Read)]
3. [WaW (Write after Write)]
4. [RaR (Read after Read)]

- The depend clause enforces additional constraints on the scheduling of tasks or loop iterations.
- These constraints establish dependences only between sibling tasks or between loop iterations.
- The task dependence is fulfilled when the predecessor task has completed.


> ## Controlling Order of Task Execution
> Consider the following loop:
>
> ~~~
> for(i=1;i<N;i++)
>   for(j=1;j<N;j++)
>     x[i][j] = x[i-1][j] + x[i][j-1];
> ~~~
> {: .source}
>
> The inner loop cannot be parallelized with `parallel for` construct due to data dependency.
>
> Use tasks with dependencies to parallelize this code. You should be able to do this by only adding OpenMP directives.
>
> Start with the following serial code:
> ~~~
> /* --- File task_depend.c --- */
> int main(int argc, char **argv) {
>
>   int N = 10;
>   int x[N][N];
>   int i,j;
>
>   /* Initialize x */
>   for(i=0;i<N;i++)
>     for(j=0;j<N;j++)
>       x[i][j]=i+j;
>
>   /* Print serial result */
>   printf("Serial result:\n");
>   for(i=1;i<N;i++){
>     for(j=1;j<N;j++){
>       x[i][j] = x[i-1][j] + x[i][j-1];
>       printf("%8d ",x[i][j]);
>     }
>     printf("\n");
>   }
>
>   /* Reset x */
>   for(i=0;i<N;i++)
>     for(j=0;j<N;j++)
>       x[i][j]=i+j;
>   printf("\n");
>
> #pragma omp parallel
>
>   /* Print parallel result */
>   printf("Parallel result:\n");
>   for(i=1;i<N;i++){
>     for(j=1;j<N;j++){
>       x[i][j] = x[i-1][j] + x[i][j-1];
>       printf("%8d ",x[i][j]);
>     }
>     printf("\n");
>   }
> }
> ~~~
> {: .source}
{: .challenge}

### Computing Fibonacci Numbers
The next example shows how  task and taskwait directives can be used to compute Fibonacci numbers recursively.

~~~
/* --- File fib.c ---*/
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int fib(int n) {
   int i, j;
   if (n<2)
      return n;
   else {
      #pragma omp task shared(i)
      i=fib(n-1);
      #pragma omp task shared(j)
      j=fib(n-2);
      #pragma omp taskwait
      return i+j;
   }
}

int main(int argc, char **argv){
   int n, result;
   char *a = argv[1];
   n = atoi(a);

   #pragma omp parallel
   {
      #pragma omp single
      result = fib(n);
   }
   printf("Result is %d\n", result);
}
~~~
{: .source}

- In the parallel construct, the single directive calls fib(n).
- The call to fib(n) generates two tasks. One of the tasks computes fib(n-1) and the other computes fib(n-2), and the return values are added together to produce the value returned by fib(n).
- Each of the calls to fib(n-1) and fib(n-2) will in turn generate two tasks. Tasks will be recursively generated until the argument passed to fib() is less than 2.
- The taskwait directive ensures that both tasks generated in fib() compute i and j before return.

> ## Task gotchas
> There are a few gotchas to be aware of. While the intention is that tasks will run in parallel, there is nothing in the specification that guarantees this behavior. You may need to check how your particular environment works.
{: .callout}

> ## Tasks?
> How do tasks work with different number of threads?
{: .challenge}
