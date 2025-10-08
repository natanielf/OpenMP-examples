#include <stdio.h>
#include <omp.h>

int main(void) {
    omp_set_max_active_levels(2); // Allows an additional level of nesting

    printf("Nested parallelism is %s\n",
           omp_get_nested() ? "supported" : "not supported"); // omp_get_nested() will return false if we do not call omp_set_max_active_levels with > 1 input

    #pragma omp parallel num_threads(4)
    {
        printf("Thread %d executes the outer parallel region\n",
               omp_get_thread_num());

        #pragma omp parallel num_threads(2)
        {
            printf("  Thread %d executes inner parallel region\n",
                   omp_get_thread_num());
        } /* end inner */
    } /* end outer */

    return 0;
}
