---
title: "Drawing the Mandelbrot set"
teaching: 20
exercises: 100
questions:
- "How do we handle irregular tasks?"
objectives:
- "Learn about the schedule() clause"
keypoints:
- "Different loop scheduling may compensate for unbalanced loop iterations"
---

Here's today's only example in Fortran. Sorry.

The <a href="https://en.wikipedia.org/wiki/Mandelbrot_set">Mandelbrot set</a>
was a hot subject of computer art in the 1980s.  The algorithm is quite simple:
For each point on the screen, do an iterative calculation and decide whether
the calculation diverges or not. Color that spot on the screen according to how
many iterations it took to diverge --- or black if it didn't diverge in 1000
iterations. 

For simplicity this implementation just prints out a number instead of coloring
a pixel.

~~~
program mandel

! Mandelbrot set generator, after
!   Chandra et al, "Parallel Programming in OpenMP", and
!   http://en.wikipedia.org/wiki/Mandelbrot_set#For_programmers
! Ross Dickson, ACEnet, 2010.06.24

integer,parameter:: maxiter=999, m=30, n=20
double precision,parameter:: xmin=-2.d0,xmax=1.d0
double precision,parameter:: ymin=-1.d0,ymax=1.d0
integer depth(n,m)
integer iter, j, k
integer starttime, endtime, tickspersec
double precision x, y

call system_clock(starttime)
!$omp parallel do private(..?...) 
do j = 1, m
    do k = 1, n
        x = xmin + j*(xmax-xmin)/m
        y = ymin + k*(ymax-ymin)/n
        depth(k, j) = mandel_val(x, y, maxiter)
    end do
end do
!$omp end parallel do
call system_clock(endtime,tickspersec)

do k = n, 1, -1
    write(*,'(30I4)') depth(k,:)
end do
write(*,*) 'working time: ',real(endtime-starttime)/real(tickspersec),' sec'

end program

integer function mandel_val(x0, y0, maxiter)
double precision x0, y0
integer maxiter
double precision x, y, xtemp
integer iter

x = 0.d0
y = 0.d0
iter = 0
do while (( x*x + y*y <= 4.d0 ) .and. ( iter < maxiter ))
   xtemp = x*x - y*y + x0
   y = 2*x*y + y0
   x = xtemp
   iter = iter + 1
end do
mandel_val = iter
end function
~~~
{: .source}

First, compile and run the program without OpenMP like so:
~~~
 gfortran mandel.f90 -o mandel-serial 
 ./mandel-serial
~~~
{: source}
Appreciate the retro ASCII art, and note how long it took to run.
A millisecond is not enough to get good performance measurements on.

Next, increase the dimensions `m,n` to `3000,2000` and recompile. You don't
want to dump 6,000,000 numbers to your screen, so send the output to a file:
`./mandel-serial >mandel-serial.out`.  
Check the run time: `tail -1 mandel-serial.out`

Now comes the parallelization.

> ## Exercise 1: Parallelize the code
>
> There is a `parallel do` directive in the code, exactly analogous to
> the `parallel for` in C. But the `private()` clause is not complete.
> Decide what variable or variables should be made private, and then 
> compile and test the code like so:
>
> ~~~
> gfortran -fopenmp mandel.f90 -o mandel-omp
> OMP_NUM_THREADS=2 ./mandel-omp >mandel-2.out
> ~~~
> {: .source}
>
> Try a few different values of `OMP_NUM_THREADS`. 
> How does the performance scale?

## The schedule() clause

OpenMP loop directives (`parallel for, parallel do`) can take several other
clauses besides the `private()` clause we've already seen. One is `schedule()`,
which allows us to specify how loop iterations are divided up among the
threads.

The default is *static* scheduling, in which all iterations are allocated to
threads before they execute any loop iterations. In *dynamic scheduling*, only
some of the iterations are allocated to threads at the beginning of the loop's
execution. Threads that complete their iterations are then eligible to get
additional work. The allocation process continues until all the iterations
have been distributed to threads. 

There's a tradeoff between overhead (i.e., how much time is spent setting up
the schedule) and load balancing (i.e., how much time is spent waiting for the
most heavily-worked thread to catch up). Static scheduling has low overhead but
may be badly balanced; dynamic scheduling has higher overhead. Both can also
take a *chunk size*; larger chunks mean less overhead and greater memory
locality, smaller chunks may mean finer load balancing. You can omit the chunk
size, it defaults to 1.

Bad load balancing might be what's causing this Mandelbrot code not to
parallelize very well. Let's add a `schedule()` clause and see what happens.

~~~
!$omp parallel do private(...?...) schedule(dynamic,?)
~~~

> ## Exercise 2: Play with the schedule() clause
> 
> Try different `schedule()` clauses and tabulate the run times with different
> thread numbers. What seems to work best for this problem?
>
> Does it change much if you grow the problem size? That is, if you make `m,n` bigger?
>
> There's a third option, `guided`, which starts with large chunks and gradually
> decreases the chunk size as it works through the iterations.
> Try it out too, if you like. With `schedule(guided,<chunk>)`, the chunk parameter
> is the smallest chunk size it will try.
