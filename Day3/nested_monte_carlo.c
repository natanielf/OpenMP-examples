// pi_nested_input.c
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#ifndef OUTER_T
#define OUTER_T 2
#endif
#ifndef INNER_T
#define INNER_T 4
#endif
#ifndef CHUNK
#define CHUNK (1ULL<<22)
#endif

// small xorshift rng
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
    omp_set_nested(1);
    omp_set_max_active_levels(2);

    unsigned long long total_inside = 0, total_points = 0;
    double t0 = omp_get_wtime();

    #pragma omp parallel num_threads(OUTER_T)
    {
        #pragma omp single
        {
            static unsigned long long next = 0;
            for(;;){
                unsigned long long start, count = CHUNK;
                #pragma omp atomic capture
                { start = next; next += CHUNK; }
                if(start >= NPTS) break;
                if(start + count > NPTS) count = NPTS - start;

                #pragma omp parallel num_threads(INNER_T)
                {
                    unsigned long long local = 0;
                    unsigned seed = 0x9e3779b9u
                                  ^ (unsigned)omp_get_thread_num()
                                  ^ (unsigned)(start & 0xffffffffu);

                    #pragma omp for schedule(static)
                    for(unsigned long long i=0;i<count;i++){
                        double x = urand(&seed);
                        double y = urand(&seed);
                        if(x*x + y*y <= 1.0) local++;
                    }
                    #pragma omp atomic
                    total_inside += local;
                }

                #pragma omp atomic
                total_points += count;
            }
        }
    }

    double t1 = omp_get_wtime();
    double pi = 4.0 * (double)total_inside / (double)total_points;
    printf("nested: pi=%.6f time=%.3fs outer=%d inner=%d chunk=%llu npts=%llu\n",
           pi, t1-t0, OUTER_T, INNER_T,
           (unsigned long long)CHUNK, (unsigned long long)NPTS);
    return 0;
}
