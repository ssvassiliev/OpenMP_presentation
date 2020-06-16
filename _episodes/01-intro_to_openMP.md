---
title:  "Introduction"
teaching: 20
exercises: 0
questions:
- "What is shared-memory programming?"
- "What is OpenMP?"
objectives:
- "Understand the shared-memory programming model and OpenMP standard"
keypoints:
- "OpenMP programs are limited to a single physical machine"
- "You use OpenMP with C, C++, or Fortran"
---

Parallel programs come in two broad flavors: shared-memory and message-passing. In this workshop, we will be looking at shared-memory programming, with a focus on Open Multi-Processing (OpenMP) programming.

## What is shared-memory programming?

OpenMP is one way of writing shared-memory parallel programs. OpenMP is actually a specification, which has been implemented by many vendors.

[OpenMP specifications](https://www.openmp.org/specifications/)

[Compilers supporting OpenMP](https://www.openmp.org/resources/openmp-compilers-tools/)

For an overview of the past, present and future of the OpenMP read the paper ["The Ongoing Evolution of OpenMP"](https://ieeexplore.ieee.org/document/8434208).

## OpenMP Execution Model

- OpenMP programs realizes parallelism through the use of threads.
- OpenMP offers the programmer full control over parallelization.
- OpenMP uses the so-called fork-join model of parallel execution:
- OpenMP divides the memory into two types: Global (or shared) memory, and thread-local memory.
![](../fig/OpenMP-execution-model.svg)

> ## Compiling C code
> ~~~
> #include <stdio.h>
> #include <stdlib.h>
>
> int main(int argc, char **argv) {
>   printf("Hello World\n");
> }
> ~~~
> {: .source}
>
> In order to compile this code, you would need to use the following command:
>
> ~~~
> gcc -o hello hello_world.c
> ~~~
> {: .bash}
>
> This gives you an executable file "hello" that will print out the text "Hello World". You run it with the command:
>
> ~~~
> ./hello
> ~~~
> {: .bash}
> ~~~
> Hello World
> ~~~
> {: .output}
> If you don't specify the output filename with the -o option compiler will use the default output name "a.out" (assembler output).
{: .callout}

> ## GCC on Compute Canada
>
> ~~~
> $ module load gcc
> $ gcc --version
> gcc (GCC) 7.3.0
> ~~~
> {: .bash}
{: .callout}

### A Very Quick Introduction to C
- Preprocessor directives
  - The `#include` directive tells the preprocessor to insert the contents of another file into the source code.
    ~~~
    #include <header_file> // Search in a predefined folders
    #include "header_file>" // Search in the current directory
    ~~~
    {: .source}

  - The `#define` directive declares constant values to be used throughout your code.
       ~~~
       # define SIZE 1000
       ~~~
       {: .source}

  - The *conditional group* `#ifdef`.

      ~~~
      #ifdef MACRO
      controlled code
      #endif /* MACRO */
      ~~~
      {: .source}

- Curly braces `{ ...  }` are used to group statements into a block of statements.

- Statement terminator is `;`

- For loop syntax:

  ~~~
  for (initialization; condition test; increment or decrement)
  {
         //Statements to be executed repeatedly
  }
  ~~~
  {: .source}

- You can skip any statements from for loop, but semicolons must be retained. For example, you can do initialization before loop:
  ~~~
  i=10;
  for(; i<100; i++)
  ~~~
  {: .source}

-  Skipping all loop statements  will result in an infinite loop: `for(;;)`

- Function definition:

  ~~~
  return_type function_name( parameter list ) {
     body of the function
  }
  ~~~
  {: .source}

  - Example:
    ~~~
    int max(int num1, int num2)
      {
         int result;
         if (num1 > num2)
            result = num1;
         else
            result = num2;
         return result;
      }
    ~~~
    {: .source}


{% include links.md %}
