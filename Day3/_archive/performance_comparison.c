// Demo 6: Performance Comparison
// Compile: gcc -fopenmp -O2 performance_comparison.c -o performance_comparison
// Run: ./performance_comparison

#include <omp.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

void compute_task(int task_id, int iterations) {
    volatile int sum = 0;
    for (int i = 0; i < iterations; i++) {
        sum += i * task_id;
    }
}

int main() {
    const int num_tasks = 1000;
    const int iterations = 100000;
    
    printf("🎬 Demo 6: Performance Comparison\n");
    printf("Running %d tasks with %d iterations each\n\n", num_tasks, iterations);
    
    // Test 1: Sequential execution
    printf("📈 Test 1: Sequential Execution\n");
    double start = omp_get_wtime();
    for (int i = 0; i < num_tasks; i++) {
        compute_task(i, iterations);
    }
    double sequential_time = omp_get_wtime() - start;
    printf("Sequential time: %.3f seconds\n\n", sequential_time);
    
    // Test 2: Parallel tasks
    printf("📈 Test 2: Parallel Tasks\n");
    start = omp_get_wtime();
    #pragma omp parallel
    #pragma omp single
    {
        for (int i = 0; i < num_tasks; i++) {
            #pragma omp task
            compute_task(i, iterations);
        }
        #pragma omp taskwait
    }
    double parallel_time = omp_get_wtime() - start;
    printf("Parallel time: %.3f seconds\n\n", parallel_time);
    
    // Results
    double speedup = sequential_time / parallel_time;
    printf("🚀 Speedup: %.2fx\n", speedup);
    printf("📊 Efficiency: %.1f%%\n", (speedup / omp_get_max_threads()) * 100);
    
    return 0;
}
