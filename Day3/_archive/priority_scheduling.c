// Demo 2: Task Priorities with Shared Memory
// Compile: gcc -fopenmp -O2 priority_scheduling.c -o priority_scheduling
// Run: OMP_NUM_THREADS=2 ./priority_scheduling

#include <omp.h>
#include <stdio.h>
#include <unistd.h>

void priority_task(int task_id, int priority, int duration_ms, int *counter, omp_lock_t *lock) {
    printf("🟢 Task %d (priority %d) STARTED on thread %d\n", 
           task_id, priority, omp_get_thread_num());

    usleep(duration_ms * 1000);

    omp_set_lock(lock);
    *counter += priority;
    printf("🔴 Task %d (priority %d) FINISHED on thread %d (counter=%d)\n", 
           task_id, priority, omp_get_thread_num(), *counter);
    omp_unset_lock(lock);
}

int main() {
    printf("🎬 Demo 2: Task Priorities\n");
    printf("Higher numbers = higher priority\n");
    printf("Counter will increment by task's priority value\n\n");
    
    int shared_counter = 0;
    omp_lock_t counter_lock;
    omp_init_lock(&counter_lock);
    
    #pragma omp parallel num_threads(2)
    #pragma omp single
    {
        // Create tasks with different priorities
        #pragma omp task priority(1)
        priority_task(1, 1, 800, &shared_counter, &counter_lock);
        
        #pragma omp task priority(10)
        priority_task(2, 10, 600, &shared_counter, &counter_lock);
        
        #pragma omp task priority(5)
        priority_task(3, 5, 400, &shared_counter, &counter_lock);
        
        #pragma omp task priority(20)
        priority_task(4, 20, 300, &shared_counter, &counter_lock);
        
        #pragma omp taskwait
    }
    
    omp_destroy_lock(&counter_lock);
    printf("\n✅ Priority demo completed! Final counter value: %d\n", shared_counter);
    return 0;
}
