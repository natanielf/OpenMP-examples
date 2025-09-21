#include <omp.h>
#include <stdio.h>

#define NUM_THREADS 4
#define N 1000

int main() {
    printf("Number of threads: %d\n", NUM_THREADS);
    int counter = 0;
    printf("Initial counter value: %d\n", counter);

#pragma omp parallel for num_threads(NUM_THREADS)
    for (int i = 0; i < N; i++) {
#pragma omp critical
        counter++;
    }

    printf("Final counter value  : %d\n", counter);
    return 0;
}
