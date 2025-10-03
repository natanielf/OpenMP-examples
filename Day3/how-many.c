#include <omp.h>
#include <stdio.h>

int main() {

    omp_set_max_active_levels(2);  // allow nested teams to be active
    omp_set_dynamic(0);            // don't auto-shrink teams

#pragma omp parallel for ordered schedule(static, 1) num_threads(12)
    for (int i = 0; i < 12; i++) {
        int L1_tid = i / 3;  // 0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3
        int L2_tid = i % 3;  // 0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2
        int L1_sz = 4;
        int L2_sz = 3;
        int level = 1;  // Single level now
        int a_tid = L1_tid;  // Same as L1_tid in this flattened approach
        int a_sz = L1_sz;

        // do nested work using L2_tid in [0..L2_sz-1]
        #pragma omp ordered
        printf("L1_tid: %d, L1_sz: %d, L2_tid: %d, L2_sz: %d, level: %d, a_tid: %d, a_sz: %d\n", L1_tid, L1_sz, L2_tid, L2_sz, level, a_tid, a_sz);
    }

}