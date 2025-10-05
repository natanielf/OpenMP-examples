// compile:  gcc -O3 -fopenmp pi_tasks.c -o pi_tasks
// run:      OMP_NUM_THREADS=8 ./pi_tasks

#include <stdio.h>
#include <omp.h>

int main() {
    const long long N = 1LL << 28;   // ~268M slices 
    const int CHUNK = 1 << 18;       // ~262k per task
    const double step = 1.0 / (double)N;

    double sum = 0.0;
    double t0 = omp_get_wtime();

    #pragma omp parallel
    #pragma omp single
    {
        for (long long start = 0; start < N; start += CHUNK) {
            long long end = (start + CHUNK < N) ? (start + CHUNK) : N;

            #pragma omp task firstprivate(start, end) shared(sum)
            {
                double local = 0.0;
                for (long long i = start; i < end; ++i) {
                    double x = (i + 0.5) * step;
                    local += 4.0 / (1.0 + x * x);
                }
                #pragma omp atomic
                sum += local;
            }
        }
        // make sure all tasks complete before using sum
        #pragma omp taskwait
    }

    double pi = sum * step;
    double t1 = omp_get_wtime();

    printf("pi ≈ %.15f\n", pi);
    printf("time: %.3f s with %d threads\n", t1 - t0, omp_get_max_threads());
    return 0;
}