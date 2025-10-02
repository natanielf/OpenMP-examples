// This file is for doing the exercise is the same as int_start.c

#include <omp.h>
#include <stdio.h>

#define NUM_THREADS 12

static long num_steps = 100000;
double step;


int main() {
    omp_set_num_threads(NUM_THREADS);
    
    double x, pi, sum = 0.0;
    
    step = 1.0 / (double)num_steps;
    
    
    #pragma omp parallel 
    {
        double thread_sum = 0.0;
        int i;
        int ID = omp_get_thread_num();
        printf("ID: %d\n", ID);

        long chunk = num_steps / NUM_THREADS;
        long start = ID * chunk;
        long end;

        if (ID == NUM_THREADS - 1) {
            end = num_steps;
        } else {
            end = (ID + 1) * chunk;
        }

        for (i = start; i < end; i++) {
            x = (i + 0.5) * step;
            thread_sum += 4.0 / (1.0 + x * x);
        }
        sum += thread_sum;
    }
    pi = step * sum;
    
    printf("Return value: %f\n", pi);

    return 0;
}