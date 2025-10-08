#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

static inline unsigned xorshift32(unsigned *s){
    unsigned x = *s;
    x ^= x << 13; x ^= x >> 17; x ^= x << 5;
    return *s = x ? x : 2463534242u;
}
static inline double urand(unsigned *s){
    return (xorshift32(s) >> 8) * (1.0/16777216.0);
}

int main(int argc, char **argv){
    if(argc < 2){
        printf("usage: %s <num_points>\n", argv[0]);
        return 1;
    }
    unsigned long long NPTS = strtoull(argv[1], NULL, 10);

    unsigned long long inside = 0;

    double t0 = omp_get_wtime();
    #pragma omp parallel
    {
        unsigned seed = 0x9e3779b9u ^ (unsigned)omp_get_thread_num();
        unsigned long long local = 0;

        #pragma omp for schedule(static)
        for (unsigned long long i = 0; i < NPTS; i++){
            double x = urand(&seed);
            double y = urand(&seed);
            if (x*x + y*y <= 1.0) local++;
        }
        #pragma omp atomic
        inside += local;
    }
    double t1 = omp_get_wtime();

    double pi = 4.0 * (double)inside / (double)NPTS;
    printf("flat:   pi=%.6f time=%.3fs threads=%d npts=%llu\n",
           pi, t1 - t0, omp_get_max_threads(), (unsigned long long)NPTS);
    return 0;
}
