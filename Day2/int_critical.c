/*
In the version we use mutex critical sections to synchronize the code. The problem
with this is that we need two critical sections which causes massive overhead
*/

#include <omp.h>
#include <stdio.h>

// Defining thread count for parallel sections
#define NUM_THREADS 12

static long num_steps = 1000000000;
double step;


int main() {
    double x, pi;

    // Integral from 0 to 1
    step = 1.0 / (double)num_steps;

    // Sets thread count for all parallel sections
    omp_set_num_threads(NUM_THREADS);

    // int thread_count = omp_get_num_threads();
    // printf("Threads: %d\n", thread_count);

    double total_sum = 0.0;

#pragma omp parallel
    {
        // Each thread gets their own sum and i variables
        double thread_sum = 0.0;
        int i;
        int ID = omp_get_thread_num();

        // Defining chunks to equally separate work done by threads
        long chunk = num_steps / NUM_THREADS;

        // start point
        long start = ID * chunk;
        long end;

        // Calculating the end point
        // Handles the leftover caused by float to int conversion
        if (ID == NUM_THREADS - 1) {
            end = num_steps;
        } else {
            end = (ID + 1) * chunk;
        }
        printf("ID: %d\n", ID);
        for (i = start; i < end; i++) {
            // Midpoint Reimann sum
            #pragma omp critical
            {
                x = (i + 0.5) * step;
                // Doing the calculation of the function
                thread_sum += 4.0 / (1.0 + x * x);
            }
        }
        // Add to total sum after loop
        #pragma omp critical
        total_sum += thread_sum;
    }

    // Multiply total sum by step to get approximation
    pi = step * total_sum;

    printf("Return value: %f\n", pi);
    return 0;
}