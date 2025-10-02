/* 
This version builds from int_nosync.c by fixing the false sharing by calculating
the pad from the cache line size. It then creates an array of the size of the
threads * the padding to eliminate any instance of false sharing
*/

#include <omp.h>
#include <stdio.h>

// Defining thread count for parallel sections
#define NUM_THREADS 12
#define CACHE_LINE_SIZE 64

// Pad: 64 / 8 = 8
#define PAD (CACHE_LINE_SIZE / sizeof(double)) 

static long num_steps = 1000000000;
double step;

// array to hold the final sums of each thread
double sum_arr[NUM_THREADS * PAD] = {0};

int main() {
    double pi;

    // Integral from 0 to 1
    step = 1.0 / (double)num_steps;

    // Sets thread count for all parallel sections
    omp_set_num_threads(NUM_THREADS);

    // Defining chunks to equally separate work done by threads
    long chunk = num_steps / NUM_THREADS;

#pragma omp parallel
    {
        // Each thread gets their own sum and i variables
        double thread_sum = 0.0;
        int i;
        int ID = omp_get_thread_num();

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
            double x = (i + 0.5) * step;
            // Doing the calculation of the function
            thread_sum += 4.0 / (1.0 + x * x);
        }
        // Once the loop is done, assign it to the respective buffer space
        sum_arr[ID * PAD] = thread_sum;
    }

    // Summing up all the partial sums
    double total_sum = 0.0;
    for (int i = 0; i < NUM_THREADS; i++) {
    	total_sum += sum_arr[i * PAD];
    }

    // Multiply total sum by step to get approximation
    pi = step * total_sum;

    printf("Return value: %f\n", pi);
    return 0;
}