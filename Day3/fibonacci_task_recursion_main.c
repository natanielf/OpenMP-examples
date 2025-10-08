// Fibonacci Task Recursion - Main Program

#include <stdio.h>
#include <omp.h>

// Function declaration
long compute_fibonacci_task(int n);

int main() {
    int n = 40;

    double t0 = omp_get_wtime();
    long ans = compute_fibonacci_task(n);
    double t1 = omp_get_wtime();

    printf("fib(%d) = %ld\n", n, ans);
    printf("time: %.3f s with %d threads\n", t1 - t0, omp_get_max_threads());
    return 0;
}
