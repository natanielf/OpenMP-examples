// Demo 5: Live Task Visualization
// Compile: gcc -fopenmp -O2 live_visualization.c -o live_visualization
// Run: OMP_NUM_THREADS=4 ./live_visualization

#include <omp.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

void animated_task(int task_id, int duration_ms) {
    printf("🎬 Task %d: ", task_id);
    fflush(stdout);
    
    for (int i = 0; i < 20; i++) {
        printf("█");
        fflush(stdout);
        usleep(duration_ms * 1000 / 20);
    }
    printf(" ✅ (thread %d)\n", omp_get_thread_num());
}

int main() {
    printf("🎮 LIVE DEMO: Real-Time Task Visualization\n");
    printf("Watch tasks execute in real-time!\n\n");
    
    #pragma omp parallel num_threads(4)
    #pragma omp single
    {
        for (int i = 1; i <= 8; i++) {
            #pragma omp task
            animated_task(i, 200);  // 200ms per task
        }
        #pragma omp taskwait
    }
    
    printf("\n🎉 All tasks completed!\n");
    return 0;
}
