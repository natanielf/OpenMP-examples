#include <omp.h>
#include <stdio.h>

// Defining thread count for parallel sections
#define NUM_THREADS 12

static long num_steps = 100000;
double step;

// array to hold the final sums of each thread
int sum_arr[NUM_THREADS];

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
		for (i = (ID * (num_steps / NUM_THREADS)); i < ((ID + 1) * (num_steps / NUM_THREADS)); i++) {
			// Midpoint Reimann sum
			x = (i + 0.5) * step;
			// Doing the calculation of the function
			sum += 4.0 / (1.0 + x * x);
		}
		// Once the loop is done, assign it to the respective buffer space
		sum_arr[ID] = sum;
	}
	// double sum = 0.0;
	// // Summing up all the partial sums
	// for (int i = 0; i < NUM_THREADS; i++) {
	// 	sum += sum_arr[i];
	// }
	
	// Multiply total sum by step to get approximation
	pi = step * sum;

	printf("Return value: %f", pi);
	return 0;
}