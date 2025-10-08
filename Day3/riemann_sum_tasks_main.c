// Riemann Sum Tasks - Main Program

#include <stdio.h>
#include <omp.h>

// Function declaration
double compute_pi_riemann_task();

int main() {
    double t0 = omp_get_wtime();
    double pi = compute_pi_riemann_task();
    double t1 = omp_get_wtime();

    printf("pi ≈ %.15f\n", pi);
    printf("time: %.3f s with %d threads\n", t1 - t0, omp_get_max_threads());
    return 0;
}
