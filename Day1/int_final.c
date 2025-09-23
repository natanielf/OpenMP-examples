/* Fixes the leftover from int_par2 and gives two options:
1. Create an array and have each thread right to a slot in the array
2. Create a total sum variable that sums all the sums from each thread 
*/

#include <omp.h>
#include <stdio.h>

// Defining thread count for parallel sections
#define NUM_THREADS 12

static long num_steps = 100000;
double step;

// // array to hold the final sums of each thread
// double sum_arr[NUM_THREADS];

int main()
{
	// x is fine here since it gets overwritten regardless of the thread running
	double x, pi;

	// Integral from 0 to 1
	step = 1.0 / (double) num_steps;

	// Sets thread count for all parallel sections
	omp_set_num_threads(NUM_THREADS);

	int thread_count = omp_get_num_threads();
	printf("Threads: %d\n", thread_count);

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
			x = (i + 0.5) * step;
			// Doing the calculation of the function
			thread_sum += 4.0 / (1.0 + x * x);
		}
		// Add to total sum after loop
		total_sum += thread_sum;
		// // Once the loop is done, assign it to the respective buffer space
		// sum_arr[ID] = sum;
	}

	// // Summing up all the partial sums
	// double sum = 0.0;
	// for (int i = 0; i < NUM_THREADS; i++) {
	// 	sum += sum_arr[i];
	// }

	// Multiply total sum by step to get approximation
	pi = step * total_sum;

	printf("Return value: %f", pi);
	return 0;
}