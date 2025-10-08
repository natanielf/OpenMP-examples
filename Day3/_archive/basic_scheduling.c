// Demo 1: Basic Task Scheduling
// Compile: gcc -fopenmp -O2 basic_scheduling.c -o basic_scheduling
// Run: OMP_NUM_THREADS=3 ./basic_scheduling

#include <omp.h>
#include <stdio.h>
#include <unistd.h>

void worker_task(int task_id, int duration_ms) {
    printf("🟢 Task %d STARTED on thread %d\n", task_id, omp_get_thread_num());
    usleep(duration_ms * 1000);  // Simulate work
    printf("🔴 Task %d FINISHED on thread %d\n", task_id, omp_get_thread_num());
}

int main() {
    printf("🎬 Demo 1: Basic Task Scheduling\n");
    printf("Threads available: %d\n\n", omp_get_max_threads());
    
    #pragma omp parallel num_threads(3)
    #pragma omp single
    {
        for (int i = 1; i <= 6; i++) {
            #pragma omp task
            worker_task(i, 500);  // 500ms each
        }
        #pragma omp taskwait
    }
    
    printf("\n✅ All tasks completed!\n");
    return 0;
}
