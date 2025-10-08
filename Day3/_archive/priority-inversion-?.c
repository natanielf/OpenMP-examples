#include <omp.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>  // for usleep

void X_priority_task(int *A, const char *id) {
    *A += omp_get_thread_num() +1; 
    printf("[%s] tid=%d start (A=%d)\n", id, omp_get_thread_num(), *A);
    
    usleep(700000);          // fake work (700 ms)
                    // "produce" A
    printf("[%s] tid=%d done (A=%d)\n", id, omp_get_thread_num(), *A);
}

int main() {
    int A = 0;

    #pragma omp parallel num_threads(2)
    #pragma omp single
    {

        #pragma omp task priority(100) depend(out: A)
        X_priority_task(&A, "high");

        #pragma omp task priority(1) depend(in: A)
        X_priority_task(&A, "low");


        #pragma omp taskwait 

        X_priority_task(&A, "end");

    }
    return 0;
}