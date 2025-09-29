// gcc -fopenmp worksharing.c -o worksharing

#include <limits.h>
#include <omp.h>
#include <stdio.h>

int main(void) {
    // Initialize shared data
    const int N = 10;
    int data[N];
    for (int i = 0; i < N; i++) {
        data[i] = (i + 1) * 10;
    }

    // Print the array
    printf("[");
    for (int i = 0; i < N; i++) {
        printf("%d", data[i]);
        if (i < N - 1) {
            printf(", ");
        }
    }
    printf("]\n");

    // Get the number of available threads
    int thread_count = omp_get_max_threads();
    printf("Your computer has %d threads available.\n", thread_count);
    // Use the maximum number of available threads
    omp_set_num_threads(thread_count);
    // Disable dynamic thread scaling
    omp_set_dynamic(0);

    printf("The parallel region is starting.\n");

    // Define a parallel block of code
#pragma omp parallel
    {
        // Only one thread (the first one that reaches it) will execute this block
#pragma omp single
        {
            int thread_id = omp_get_thread_num();
            printf("Thread %d is executing the first SINGLE block (only one thread does this).\n", thread_id);
        }

        // Only one thread (the first one that reaches it) will execute this block
#pragma omp single
        {
            int thread_id = omp_get_thread_num();
            printf("Thread %d is executing the second SINGLE block (only one thread does this).\n", thread_id);
        }

        // Only one thread (master thread) will execute this block
#pragma omp master
        {
            int thread_id = omp_get_thread_num(); // The master thread always has an id of 0
            printf("Thread %d is executing the MASTER block (only the master thread does this).\n", thread_id);
        }

        // The team splits the work into three independent sections (task parallelism)
#pragma omp sections
        {
            // Section 0: Calculate the sum of the array
#pragma omp section
            {
                int sum = 0;
                for (int i = 0; i < N; i++) {
                    sum += data[i];
                }
                int thread_id = omp_get_thread_num();
                printf("(Section 0) Thread %d: sum = %d\n", thread_id, sum);
            }

            // Section 1: Find the minimum and maximum values in the array
#pragma omp section
            {
                int min = INT_MAX;
                int max = INT_MIN;
                for (int i = 0; i < N; i++) {
                    int value = data[i];
                    if (value < min) {
                        min = value;
                    }
                    if (value > max) {
                        max = value;
                    }
                }
                int thread_id = omp_get_thread_num();
                printf("(Section 1) Thread %d: min = %d, max = %d\n", thread_id, min, max);
            }

            // Section 2: Calculate the average of all values stored in the array
#pragma omp section
            {
                int sum = 0;
                for (int i = 0; i < N; i++) {
                    sum += data[i];
                }
                double avg = (double)sum / (double)N;
                int thread_id = omp_get_thread_num();
                printf("(Section 2) Thread %d: avg = %f\n", thread_id, avg);
            }
        }

        // Each thread processes a subset of the array indices
#pragma omp for
        for (int i = 0; i < N; i++) {
            // The loop body is executed by the thread that owns this iteration
            int thread_id = omp_get_thread_num();
            printf("Thread %d processes element at index %d: value = %d\n", thread_id, i, data[i]);
        }
    }

    printf("The parallel region has ended.\n");

    return 0;
}
