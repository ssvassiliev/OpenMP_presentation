---
title: "Calculating Fibonacci numbers"
teaching: 10
exercises: 5
questions:
- "How to recurse in parallel"
objectives:
- "Use general parallel tasks"
keypoints:
- "OpenMP can manage general parallel tasks"
---

The last technique we will look at is the task structure in OpenMP. In this
case, you can use the `task` keyword within a parallel section to define sets of
tasks that get queued up to run within the threads of the parallel section. They
could run in any order. If you need to have results from the tasks before you
can continue, you can use the `taskwait` keyword to tell OpenMP to pause your
program until the tasks are done. This would look like the following:

~~~
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

In the main function, you need to start the parallel section so that all of the threads are launched. Since we only want the parent call to fib done once, we need to use the single keyword.

> ## Task gotchas 
> There are a few gotchas to be aware of. While the intention is that tasks
> will run in parallel, there is nothing in the specification that guarantees
> this behavior. You may need to check how your particular environment works.
{: .callout}

> ## Tasks?
> How do tasks work with different number of threads?
{: .challenge}
