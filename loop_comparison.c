// gcc -fopenmp loop_comparison.c -o loop_comparison

#include <omp.h>
#include <stdio.h>

#define NUM_THREADS 8
#define ARRAY_SIZE 10000000 // 10 million elements
int data[ARRAY_SIZE];       // Declare an array of integers

int main() {
    double start_time;
    double end_time;

    // Sequential loop
    start_time = omp_get_wtime();
    for (int i = 0; i < ARRAY_SIZE; i++) {
        data[i] = i * 2;
    }
    end_time = omp_get_wtime();
    double seq_time = end_time - start_time;
    printf("Sequential time:           %.8lf seconds\n", seq_time);

    // ------------------------------------------------------------

    // Parallel loop
    start_time = omp_get_wtime();
#pragma omp parallel for num_threads(NUM_THREADS)
    for (int i = 0; i < ARRAY_SIZE; i++) {
        data[i] = i * 2;
    }
    end_time = omp_get_wtime();
    double par_time = end_time - start_time;
    printf("Parallel time (%d threads): %.8lf seconds\n", NUM_THREADS, par_time);

    // Calculate the speedup
    if (par_time > 0) {
        double speedup = seq_time / par_time;
        printf("Speedup: %.2fx\n", speedup);
    }
}
