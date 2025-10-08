// Demo 3: Task Dependencies
// Compile: gcc -fopenmp -O2 dependency_scheduling.c -o dependency_scheduling
// Run: OMP_NUM_THREADS=3 ./dependency_scheduling

#include <omp.h>
#include <stdio.h>
#include <unistd.h>

int shared_data = 0;

void dependent_task(int task_id, int duration_ms) {
    printf("🟢 Task %d STARTED on thread %d (data=%d)\n", 
           task_id, omp_get_thread_num(), shared_data);
    usleep(duration_ms * 1000);
    shared_data += task_id;
    printf("🔴 Task %d FINISHED on thread %d (data=%d)\n", 
           task_id, omp_get_thread_num(), shared_data);
}

int main() {
    printf("🎬 Demo 3: Task Dependencies\n");
    printf("Tasks must wait for their dependencies!\n\n");
    
    #pragma omp parallel num_threads(3)
    #pragma omp single
    {
        // Task 1: Produces data
        #pragma omp task depend(out: shared_data)
        dependent_task(1, 500);
        
        // Task 2: Depends on Task 1
        #pragma omp task depend(in: shared_data)
        dependent_task(2, 300);
        
        // Task 3: Also depends on Task 1
        #pragma omp task depend(in: shared_data)
        dependent_task(3, 400);
        
        // Task 4: Depends on Task 2
        #pragma omp task depend(in: shared_data)
        dependent_task(4, 200);
        
        #pragma omp taskwait
    }
    
    printf("\n✅ Dependency demo completed!\n");
    return 0;
}
