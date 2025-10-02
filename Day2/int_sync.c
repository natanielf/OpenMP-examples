/*
This is the most correct form of synchronization using worksharing constructs
and the reduction synchronization construct. Unlike int_nosync.c, this version
does not have any false sharing
*/

#include <omp.h>
#include <stdio.h>

// Defining thread count for parallel sections
#define NUM_THREADS 12

static long num_steps = 1000000000;
double step;

int main() {
    double pi;

    // Integral from 0 to 1
    step = 1.0 / (double)num_steps;

    // Sets thread count for all parallel sections
    omp_set_num_threads(NUM_THREADS);

    double total_sum = 0.0;
       
    #pragma omp parallel for reduction(+:total_sum)
    for (long i = 0; i < num_steps; i++) {
        // Midpoint Reimann sum
        double x = (i + 0.5) * step;
        // Doing the calculation of the function
        total_sum += 4.0 / (1.0 + x * x);
    }

    // Multiply total sum by step to get approximation
    pi = step * total_sum;

    printf("Return value: %f\n", pi);
    return 0;
}