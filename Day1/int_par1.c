/* Bad attempt at parallelization by just putting a parallel section but not changing any of the code. 
This version will give variable outputs for pi which is not supposed to happen */

#include <omp.h>
#include <stdio.h>

// Defining thread count for parallel sections
#define NUM_THREADS 12

int i;
static long num_steps = 100000;
double step;

int main() {
	double x, pi, sum = 0.0;

	// Integral from 0 to 1
	step = 1.0 / (double) num_steps;

	// Sets thread count for all parallel sections
	omp_set_num_threads(NUM_THREADS);

	#pragma omp parallel
	{
		for (i = 0; i < num_steps; i++) {
			// Midpoint Reimann sum
			x = (i + 0.5) * step;
			// Doing the calculation of the function
			sum = sum + 4.0 / (1.0 + x * x);
		}
	}

	// Multiply total sum by step to get approximation
	pi = step * sum;

	printf("Return value: %f", pi);

	return 0;
}