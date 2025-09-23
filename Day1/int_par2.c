#include <omp.h>
#include <stdio.h>

static long num_steps = 100000;
// int i;
double step;

#define NUM_THREADS 12

int sum_arr[NUM_THREADS];

int main()
{
	double x, pi;
	double sum = 0.0;

	step = 1.0 / (double) num_steps;

	omp_set_num_threads(NUM_THREADS);

	int thread_count = omp_get_num_threads();
	printf("Threads: %d\n", thread_count);
	
	#pragma omp parallel 
	{
		int i;
		int ID = omp_get_thread_num();
		printf("ID: %d\n", ID);
		for (i = (ID * (num_steps / NUM_THREADS)); i < ((ID + 1) * (num_steps / NUM_THREADS)); i++) {
			x = (i + 0.5) * step;
			sum += 4.0 / (1.0 + x * x);
		}
		// sum_arr[ID] = sum;
	}
	// for (i = 0; i < num_steps; i++) {
	// 	x = (i + 0.5) * step;
	// 	sum = sum + 4.0 / (1.0 + x * x);
	// }
	// double sum = 0.0;
	// for (int i = 0; i < NUM_THREADS; i++) {
	// 	sum += sum_arr[i];
	// }
	pi = step * sum;

	printf("Return value: %f", pi);
	return 0;
}