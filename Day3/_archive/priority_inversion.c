// Demo 4: Priority Inversion Problem
// Compile: gcc -fopenmp -O2 priority_inversion.c -o priority_inversion
// Run: OMP_NUM_THREADS=2 ./priority_inversion

#include <omp.h>
#include <stdio.h>
#include <unistd.h>

int critical_resource = 0;

void high_priority_task() {
    printf("🔥 HIGH priority task STARTED on thread %d\n", omp_get_thread_num());
    usleep(200000);  // 200ms
    printf("🔥 HIGH priority task FINISHED on thread %d\n", omp_get_thread_num());
}

void low_priority_task() {
    printf("🐌 LOW priority task STARTED on thread %d\n", omp_get_thread_num());
    usleep(1000000);  // 1000ms - long task
    printf("🐌 LOW priority task FINISHED on thread %d\n", omp_get_thread_num());
}

void medium_priority_task() {
    printf("⚡ MEDIUM priority task STARTED on thread %d\n", omp_get_thread_num());
    usleep(300000);  // 300ms
    printf("⚡ MEDIUM priority task FINISHED on thread %d\n", omp_get_thread_num());
}

int main() {
    printf("🎬 Demo 4: Priority Inversion Problem\n");
    printf("Watch what happens with limited threads!\n\n");
    
    #pragma omp parallel num_threads(2)  // Limited threads!
    #pragma omp single
    {
        // Create tasks in priority order
        #pragma omp task priority(100) depend(out: critical_resource)
        high_priority_task();
        
        #pragma omp task priority(1) depend(in: critical_resource)
        low_priority_task();
        
        #pragma omp task priority(50)
        medium_priority_task();
        
        #pragma omp task priority(50)
        medium_priority_task();
        
        #pragma omp taskwait
    }
    
    printf("\n✅ Priority inversion demo completed!\n");
    return 0;
}
