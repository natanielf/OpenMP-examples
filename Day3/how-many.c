#include <omp.h>
#include <stdio.h>

#define MAX_DEPTH 3

void recursive_task(int depth, int max_depth, int *task_count) {
    if (depth >= max_depth) return;
    
    #pragma omp task
    recursive_task(depth + 1, max_depth, task_count);
    
    #pragma omp task
    recursive_task(depth + 1, max_depth, task_count);
    
    #pragma omp atomic
    (*task_count)++;
}

int main() {
    int task_count = 0;
    
    #pragma omp parallel
    #pragma omp single
    {
        recursive_task(0, MAX_DEPTH, &task_count);
        #pragma omp taskwait
    }
    
    printf("Total tasks created: %d\n", task_count);
    return 0;
}



