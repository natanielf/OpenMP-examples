// Concurrent Tasks Demo - Fibonacci and Pi Computation
//
// This program demonstrates running two different computational tasks concurrently:
// 1. Fibonacci computation using recursive tasks with cutoff strategy
// 2. Pi estimation using Riemann sum with task-based parallelism
//
// Key OpenMP Concepts Demonstrated:
// - Nested parallelism: tasks within tasks
// - Concurrent execution of different algorithms
// - Task synchronization with taskwait
// - Performance comparison of concurrent vs sequential execution

#include <stdio.h>
#include <omp.h>

// Function declarations
long compute_fibonacci_task(int n);
double compute_pi_riemann_task();

int main() {
    const int fib_n = 40;
    long fib_result;
    double pi_result;
    
    // Sequential execution 
    printf("Sequential Execution:\n");
    double seq_start = omp_get_wtime();
    
    fib_result = compute_fibonacci_task(fib_n);
    pi_result = compute_pi_riemann_task();
    
    double seq_time = omp_get_wtime() - seq_start;
    
    printf("Results:\n");
    printf("  Fibonacci(%d) = %ld\n", fib_n, fib_result);
    printf("  Pi ≈ %.15f\n", pi_result);
    printf("  Sequential time: %.3f seconds\n\n", seq_time);
    
    printf("========================\n");

    // Concurrent execution using tasks
    printf("Concurrent Execution:\n");
    double conc_start = omp_get_wtime();
    
    #pragma omp parallel
    #pragma omp single
    {
        // Create task for Fibonacci computation
        #pragma omp task shared(fib_result)
        {
            printf("  Task 1 (Fibonacci) started on thread %d\n", omp_get_thread_num());
            fib_result = compute_fibonacci_task(fib_n);
            printf("  Task 1 (Fibonacci) completed");
        }
        
        // Create task for Pi computation
        #pragma omp task shared(pi_result)
        {
            printf("  Task 2 (Pi) started on thread %d\n", omp_get_thread_num());
            pi_result = compute_pi_riemann_task();
            printf("  Task 2 (Pi) completed");
        }
        
        // Wait for both tasks to complete
        #pragma omp taskwait
    }
    
    double conc_time = omp_get_wtime() - conc_start;
    
    printf("\nConcurrent Results:\n");
    printf("  Fibonacci(%d) = %ld\n", fib_n, fib_result);
    printf("  Pi ≈ %.15f\n", pi_result);
    printf("  Concurrent time: %.3f seconds\n\n", conc_time);
    
    // Performance analysis
    printf("=======================\n");
    printf("Sequential time:  %.3f seconds\n", seq_time);
    printf("Concurrent time:  %.3f seconds\n", conc_time);
    printf("Speedup:          %.2fx\n", seq_time / conc_time);
    printf("Efficiency:       %.1f%%\n", (seq_time / conc_time) / omp_get_max_threads() * 100);

    return 0;
}
