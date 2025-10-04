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

int main() {
    int n = 40;
    long ans;

    #pragma omp parallel
    {
        #pragma omp single   // one thread seeds the recursion
        ans = fib(n);
    }

    printf("fib(%d) = %ld\n", n, ans);
    return 0;
}