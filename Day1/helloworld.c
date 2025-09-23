#include <omp.h>
#include <stdio.h>

int main() 
{
    // Sets the amount of threads to run for all parallel sections
	omp_set_num_threads(4); //omp_get_max_threads() gives 12 for 12 logical processors

    // Parallel section
    #pragma omp parallel 
    {
        int ID = omp_get_thread_num();
		printf("hello(%d), ", ID);
		printf("world(%d)\n", ID);
    }
    return 0;
}
