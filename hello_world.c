// gcc -fopenmp hello_world.c -o hello_world

// Include the OpenMP header file
#include <omp.h>
#include <stdio.h>

// Manually define the number of threads to run in parallel
#define NUM_THREADS 4

int main() {
    // Get the number of available processors
    int num_procs = omp_get_num_procs();
    printf("%d processors available (using %d threads)\n", num_procs, NUM_THREADS);

    printf("\n");

    // ------------------------------------------------------------

    // Run a single statement in parallel using multiple threads
#pragma omp parallel num_threads(NUM_THREADS)
    printf("Hello, World!\n");

    printf("\n");

    // ------------------------------------------------------------

    // Declare a parallel for loop
#pragma omp parallel for num_threads(NUM_THREADS)
    for (int i = 0; i < NUM_THREADS + 1; i++) {
        // Get the current thread number
        int thread_number = omp_get_thread_num();
        printf("Thread %d: Hello, World!\n", thread_number);
    }

    printf("\n");

    // ------------------------------------------------------------

    // Declare a parallel code block
#pragma omp parallel num_threads(NUM_THREADS)
    {
        int thread_num = omp_get_thread_num();
        printf("Thread %d is starting\n", thread_num);

        if (thread_num % 2 == 0) {
            printf("    Thread number %d is even\n", thread_num);
        } else {
            printf("    Thread number %d is odd\n", thread_num);
        }

        // Count to 10 using multiple threads
#pragma omp for
        for (int i = 0; i < 10; i++) {
            printf("    i = %d (Thread %d)\n", i, thread_num);
        }
        printf("Thread %d is finishing\n", thread_num);
    }
}
