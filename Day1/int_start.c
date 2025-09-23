// Serial program of an integral calculation for (4 / (1 + x^2)) from 0 to 1

#include <omp.h>
#include <stdio.h>

int i;
static long num_steps = 100000;
double step;

int main() {
    double x, pi, sum = 0.0;

    // Integral from 0 to 1
    step = 1.0 / (double)num_steps;

    for (i = 0; i < num_steps; i++) {
        // Midpoint Reimann sum
        x = (i + 0.5) * step;
        // Doing the calculation of the function
        sum = sum + 4.0 / (1.0 + x * x);
    }

    // Multiply total sum by step to get approximation
    pi = step * sum;

    printf("Return value: %f\n", pi);

    return 0;
}