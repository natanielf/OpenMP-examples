#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

// Multithreaded matrix multiplication benchmark
int main() {
    int proc_count = omp_get_num_procs();

    for (int N = 10; N <= 1000; N *= 10) {
        double *A = malloc(N * N * sizeof(double));
        double *B = malloc(N * N * sizeof(double));
        double *C = malloc(N * N * sizeof(double));

        if (!A || !B || !C) {
            printf("Memory allocation failed!\n");
            return 1;
        }

        // Initialize matrices
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++) {
                A[i * N + j] = 1.0;
                B[i * N + j] = 2.0;
                C[i * N + j] = 0.0;
            }

        double base_time = 0.0;

        printf("Benchmarking matrix multiplication (size %d x %d)\n", N, N);
        printf("%-10s %-15s %-15s\n", "Threads", "Time (s)", "Speedup");

        for (int threads = 1; threads <= proc_count; threads *= 2) {
            omp_set_num_threads(threads);

            double start = omp_get_wtime();

#pragma omp parallel for collapse(2)
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    double sum = 0.0;
                    for (int k = 0; k < N; k++) {
                        sum += A[i * N + k] * B[k * N + j];
                    }
                    C[i * N + j] = sum;
                }
            }

            double end = omp_get_wtime();
            double elapsed = end - start;

            if (threads == 1) {
                base_time = elapsed;
            }

            printf("%-10d %-15.5f %-15.2f\n", threads, elapsed, base_time / elapsed);
        }

        printf("Check: C[0][0] = %.2f, C[N-1][N-1] = %.2f\n", C[0], C[(N - 1) * N + (N - 1)]);
        printf("\n");

        // Free memory
        free(A);
        free(B);
        free(C);
    }
    return 0;
}
