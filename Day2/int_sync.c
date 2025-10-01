#include <omp.h>
#include <stdio.h>

// Defining thread count for parallel sections
#define NUM_THREADS 12

static long num_steps = 1000000000;
double step;

// // array to hold the final sums of each thread
// double sum_arr[NUM_THREADS] = {0};

int main() {
    // x is fine here since it gets overwritten regardless of the thread running
    double pi;

    // Integral from 0 to 1
    step = 1.0 / (double)num_steps;

    // Sets thread count for all parallel sections
    omp_set_num_threads(NUM_THREADS);

    // int thread_count = omp_get_num_threads();
    // printf("Threads: %d\n", thread_count);

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