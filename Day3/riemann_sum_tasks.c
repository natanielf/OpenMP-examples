// Pi Estimation using Riemann Sum with OpenMP Tasks
// 
// This program estimates π using numerical integration:
// π = ∫₀¹ 4/(1+x²) dx ≈ Σᵢ₌₀ⁿ 4/(1+xᵢ²) * Δx
// 
// Where:
// - xᵢ = (i + 0.5) * step (midpoint rule)
// - step = 1/N (width of each rectangle)
// - N = 2²⁸ ≈ 268 million slices for high precision
//
// The computation is parallelized using OpenMP tasks:
// - Each task processes a chunk of ~262k slices
// - Tasks use atomic operations to safely update the global sum
// - taskwait ensures all tasks complete before final calculation
//
// compile:  gcc -fopenmp taskwait.c -o taskwait
//           clang -fopenmp taskwait.c -o taskwait
// run:      OMP_NUM_THREADS=8 ./taskwait

#include <stdio.h>
#include <omp.h>

// Entry point function that can be called as a task
double compute_pi_riemann_task() {
    const long long N = 1LL << 28;   // ~268M slices 
    const int CHUNK = 1 << 18;       // ~262k per task
    const double step = 1.0 / (double)N;

    double sum = 0.0;

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

    return sum * step;
}
