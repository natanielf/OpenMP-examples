// This file is for doing the exercise is the same as int_start.c

#include <omp.h>
#include <stdio.h>

int i;
static long num_steps = 100000;
double step;

int main() {
	double x, pi, sum = 0.0;

	step = 1.0 / (double) num_steps;

	for (i = 0; i < num_steps; i++) {
		x = (i + 0.5) * step;
		sum = sum + 4.0 / (1.0 + x * x);
	}
	pi = step * sum;

	printf("Return value: %f", pi);

	return 0;
}