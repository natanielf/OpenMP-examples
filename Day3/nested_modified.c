#include <stdio.h>
#include <omp.h>

int main(void) {
    omp_set_max_active_levels(2);

    printf("Nested parallelism is %s\n",
           omp_get_nested() ? "supported" : "not supported"); 

    int TID = -1; // declared in enclosing scope for private(TID)
    
    #pragma omp parallel num_threads(4) private(TID)
    {
        TID = omp_get_thread_num();
        printf("Thread %d executes the outer parallel region\n", TID);

        #pragma omp parallel num_threads(2) firstprivate(TID)
        {
            printf("TID %d: Thread %d executes inner parallel region\n",
                   TID, omp_get_thread_num());
        } /* end inner */
    } /* end outer */

    return 0;
}
