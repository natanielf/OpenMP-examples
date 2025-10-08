// Fibonacci Computation using OpenMP Tasks with Cutoff Strategy
//
// compile:  gcc -fopenmp recursion.c -o recursion
//          clang -fopenmp recursion.c -o recursion
// run:     OMP_NUM_THREADS=8 ./recursion

#include <stdio.h>
#include <omp.h>

long fib(int n) {
    if (n < 2) return n;

    long x, y;

    // spawn tasks only when n is big enough (cutoff)
    #pragma omp task shared(x) if(n > 20)
    x = fib(n - 1);

    #pragma omp task shared(y) if(n > 20)
    y = fib(n - 2);

    #pragma omp taskwait
    return x + y;
}

// Entry point function that can be called as a task
long compute_fibonacci_task(int n) {
    long ans;
    
    #pragma omp parallel
    {
        #pragma omp single
        ans = fib(n);
    }
    
    return ans;
}
