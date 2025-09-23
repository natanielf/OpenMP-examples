// This version will correctly calculate pi for thread counts that give a whole number when dividing

#include <omp.h>
#include <stdio.h>

// Defining thread count for parallel sections
#define NUM_THREADS 4 // Will fail for 12 threads

static long num_steps = 100000;
double step;

int main()
{
	// x is fine here since it gets overwritten regardless of the thread running
	double x, pi;
	double sum = 0.0;

	// Integral from 0 to 1
	step = 1.0 / (double) num_steps;

	// Sets thread count for all parallel sections
	omp_set_num_threads(NUM_THREADS);

	int thread_count = omp_get_num_threads();
	printf("Threads: %d\n", thread_count);
	
	#pragma omp parallel 
	{
		int i;
		int ID = omp_get_thread_num();
		printf("ID: %d\n", ID);
		long chunk = num_steps / NUM_THREADS;
		for (i = (ID * chunk); i < ((ID + 1) * chunk); i++) {
			// Midpoint Reimann sum
			x = (i + 0.5) * step;
			// Doing the calculation of the function
			sum += 4.0 / (1.0 + x * x);
		}
	}
	
	// Multiply total sum by step to get approximation
	pi = step * sum;

	printf("Return value: %f", pi);
	return 0;
}