#include <omp.h>
#include <stdio.h>

static long num_steps = 100000;
double step;

#define NUM_THREADS 12

double sum_arr[NUM_THREADS];

int main()
{
	double x, pi;

	step = 1.0 / (double) num_steps;

	omp_set_num_threads(NUM_THREADS);

	int thread_count = omp_get_num_threads();
	printf("Threads: %d\n", thread_count);
	
    	#pragma omp parallel 
		{
			double sum = 0.0;
			int i;
			int ID = omp_get_thread_num();
			long chunk = num_steps / NUM_THREADS;
			long start = ID * chunk;
			long end;
			if (ID == NUM_THREADS - 1) {
				end = num_steps;
			} else {
				end = (ID + 1) * chunk;
			}
			printf("ID: %d\n", ID);
			for (i = start; i < end; i++) {
				x = (i + 0.5) * step;
				sum += 4.0 / (1.0 + x * x);
			}
			sum_arr[ID] = sum;
		}

	double sum = 0.0;
	for (int i = 0; i < NUM_THREADS; i++) {
		sum += sum_arr[i];
	}
	pi = step * sum;

	printf("Return value: %f", pi);
	return 0;
}