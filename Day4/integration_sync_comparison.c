// Integration Synchronization Methods Comparison
// Uses proven functions from Day2 examples
// Compile: clang -fopenmp -O2 integration_sync_comparison.c -o integration_sync_comparison -lm
// Run: OMP_NUM_THREADS=8 ./integration_sync_comparison

#include <omp.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#define NUM_THREADS 8
#define NUM_STEPS 100000000
#define CACHE_LINE_SIZE 64
#define PAD (CACHE_LINE_SIZE / sizeof(double))

// Global variables for different methods
static double step;
static double sum_arr[NUM_THREADS * PAD] = {0};

// Method 1: Critical Section (from int_critical.c)
double pi_critical_section() {
    double total_sum = 0.0;
    
    #pragma omp parallel num_threads(NUM_THREADS)
    {
        double thread_sum = 0.0;
        int ID = omp_get_thread_num();
        long chunk = NUM_STEPS / NUM_THREADS;
        long start = ID * chunk;
        long end = (ID == NUM_THREADS - 1) ? NUM_STEPS : (ID + 1) * chunk;
        
        for (long i = start; i < end; i++) {
            #pragma omp critical
            {
                double x = (i + 0.5) * step;
                thread_sum += 4.0 / (1.0 + x * x);
            }
        }
        
        #pragma omp critical
        total_sum += thread_sum;
    }
    
    return step * total_sum;
}

// Method 2: No Synchronization - Race Condition (from int_nosync.c but with race)
double pi_no_sync_race() {
    double total_sum = 0.0;
    
    #pragma omp parallel num_threads(NUM_THREADS)
    {
        double thread_sum = 0.0;
        int ID = omp_get_thread_num();
        long chunk = NUM_STEPS / NUM_THREADS;
        long start = ID * chunk;
        long end = (ID == NUM_THREADS - 1) ? NUM_STEPS : (ID + 1) * chunk;
        
        for (long i = start; i < end; i++) {
            double x = (i + 0.5) * step;
            thread_sum += 4.0 / (1.0 + x * x);
        }
        
        // RACE CONDITION: Multiple threads updating total_sum without synchronization
        // This will cause lost updates!
        double temp = total_sum;
        // Add some work to make race condition more likely
        for (int j = 0; j < 100; j++) {
            temp += 0.0;  // Dummy work
        }
        temp += thread_sum;
        total_sum = temp;
    }
    
    return step * total_sum;
}

// Method 3: Proper No Sync (from int_nosync.c)
double pi_no_sync_proper() {
    double sum_arr_local[NUM_THREADS] = {0};
    
    #pragma omp parallel num_threads(NUM_THREADS)
    {
        double thread_sum = 0.0;
        int ID = omp_get_thread_num();
        long chunk = NUM_STEPS / NUM_THREADS;
        long start = ID * chunk;
        long end = (ID == NUM_THREADS - 1) ? NUM_STEPS : (ID + 1) * chunk;
        
        for (long i = start; i < end; i++) {
            double x = (i + 0.5) * step;
            thread_sum += 4.0 / (1.0 + x * x);
        }
        
        sum_arr_local[ID] = thread_sum;
    }
    
    // Sum up all partial sums
    double total_sum = 0.0;
    for (int i = 0; i < NUM_THREADS; i++) {
        total_sum += sum_arr_local[i];
    }
    
    return step * total_sum;
}

// Method 4: False Sharing Fix (from int_falsefix.c)
double pi_false_sharing_fix() {
    #pragma omp parallel num_threads(NUM_THREADS)
    {
        double thread_sum = 0.0;
        int ID = omp_get_thread_num();
        long chunk = NUM_STEPS / NUM_THREADS;
        long start = ID * chunk;
        long end = (ID == NUM_THREADS - 1) ? NUM_STEPS : (ID + 1) * chunk;
        
        for (long i = start; i < end; i++) {
            double x = (i + 0.5) * step;
            thread_sum += 4.0 / (1.0 + x * x);
        }
        
        sum_arr[ID * PAD] = thread_sum;
    }
    
    // Sum up all partial sums
    double total_sum = 0.0;
    for (int i = 0; i < NUM_THREADS; i++) {
        total_sum += sum_arr[i * PAD];
    }
    
    return step * total_sum;
}

// Method 5: Reduction (from int_sync.c)
double pi_reduction() {
    double total_sum = 0.0;
    
    #pragma omp parallel for num_threads(NUM_THREADS) reduction(+:total_sum)
    for (long i = 0; i < NUM_STEPS; i++) {
        double x = (i + 0.5) * step;
        total_sum += 4.0 / (1.0 + x * x);
    }
    
    return step * total_sum;
}

// Method 6: Atomic Operations
double pi_atomic() {
    double total_sum = 0.0;
    
    #pragma omp parallel num_threads(NUM_THREADS)
    {
        double thread_sum = 0.0;
        int ID = omp_get_thread_num();
        long chunk = NUM_STEPS / NUM_THREADS;
        long start = ID * chunk;
        long end = (ID == NUM_THREADS - 1) ? NUM_STEPS : (ID + 1) * chunk;
        
        for (long i = start; i < end; i++) {
            double x = (i + 0.5) * step;
            thread_sum += 4.0 / (1.0 + x * x);
        }
        
        #pragma omp atomic
        total_sum += thread_sum;
    }
    
    return step * total_sum;
}

// Reference calculation (sequential)
double pi_reference() {
    double total_sum = 0.0;
    
    for (long i = 0; i < NUM_STEPS; i++) {
        double x = (i + 0.5) * step;
        total_sum += 4.0 / (1.0 + x * x);
    }
    
    return step * total_sum;
}

int main() {
    printf("🎬 Integration Synchronization Methods Comparison\n");
    printf("================================================\n\n");
    
    printf("Computing π using numerical integration: ∫₀¹ 4/(1+x²) dx\n");
    printf("Steps: %d, Threads: %d\n", NUM_STEPS, NUM_THREADS);
    printf("Expected result: π ≈ 3.141592653589793...\n\n");
    
    // Initialize step size
    step = 1.0 / (double)NUM_STEPS;
    
    // Compute reference result
    printf("📐 Computing reference result...\n");
    double start_ref = omp_get_wtime();
    double reference_pi = pi_reference();
    double ref_time = omp_get_wtime() - start_ref;
    printf("Reference π: %.15f (computed in %.3f seconds)\n\n", reference_pi, ref_time);
    
    // Test 1: Critical Section
    printf("🔒 Test 1: Critical Section (from int_critical.c)\n");
    printf("------------------------------------------------\n");
    double start = omp_get_wtime();
    double pi_critical = pi_critical_section();
    double critical_time = omp_get_wtime() - start;
    printf("Critical time: %.3f seconds, π: %.15f\n", critical_time, pi_critical);
    printf("Error: %.2e (%.4f%%)\n\n", 
           fabs(pi_critical - reference_pi), 
           fabs(pi_critical - reference_pi) / reference_pi * 100);
    
    // Test 2: Race Condition (BAITED!)
    printf("💥 Test 2: Race Condition (BAITED!)\n");
    printf("----------------------------------\n");
    start = omp_get_wtime();
    double pi_race = pi_no_sync_race();
    double race_time = omp_get_wtime() - start;
    printf("Race time: %.3f seconds, π: %.15f\n", race_time, pi_race);
    printf("Error: %.2e (%.4f%%)\n", 
           fabs(pi_race - reference_pi), 
           fabs(pi_race - reference_pi) / reference_pi * 100);
    printf("Race condition detected: %s\n\n", 
           fabs(pi_race - reference_pi) > 1e-10 ? "YES!" : "NO");
    
    // Test 3: Proper No Sync
    printf("✅ Test 3: Proper No Sync (from int_nosync.c)\n");
    printf("--------------------------------------------\n");
    start = omp_get_wtime();
    double pi_no_sync = pi_no_sync_proper();
    double no_sync_time = omp_get_wtime() - start;
    printf("No sync time: %.3f seconds, π: %.15f\n", no_sync_time, pi_no_sync);
    printf("Error: %.2e (%.4f%%)\n\n", 
           fabs(pi_no_sync - reference_pi), 
           fabs(pi_no_sync - reference_pi) / reference_pi * 100);
    
    // Test 4: False Sharing Fix
    printf("🚀 Test 4: False Sharing Fix (from int_falsefix.c)\n");
    printf("------------------------------------------------\n");
    start = omp_get_wtime();
    double pi_false_fix = pi_false_sharing_fix();
    double false_fix_time = omp_get_wtime() - start;
    printf("False fix time: %.3f seconds, π: %.15f\n", false_fix_time, pi_false_fix);
    printf("Error: %.2e (%.4f%%)\n\n", 
           fabs(pi_false_fix - reference_pi), 
           fabs(pi_false_fix - reference_pi) / reference_pi * 100);
    
    // Test 5: Reduction
    printf("🎯 Test 5: Reduction (from int_sync.c)\n");
    printf("-------------------------------------\n");
    start = omp_get_wtime();
    double pi_reduction_result = pi_reduction();
    double reduction_time = omp_get_wtime() - start;
    printf("Reduction time: %.3f seconds, π: %.15f\n", reduction_time, pi_reduction_result);
    printf("Error: %.2e (%.4f%%)\n\n", 
           fabs(pi_reduction_result - reference_pi), 
           fabs(pi_reduction_result - reference_pi) / reference_pi * 100);
    
    // Test 6: Atomic Operations
    printf("⚛️  Test 6: Atomic Operations\n");
    printf("----------------------------\n");
    start = omp_get_wtime();
    double pi_atomic_result = pi_atomic();
    double atomic_time = omp_get_wtime() - start;
    printf("Atomic time: %.3f seconds, π: %.15f\n", atomic_time, pi_atomic_result);
    printf("Error: %.2e (%.4f%%)\n\n", 
           fabs(pi_atomic_result - reference_pi), 
           fabs(pi_atomic_result - reference_pi) / reference_pi * 100);
    
    // Performance comparison
    printf("📊 Performance Comparison\n");
    printf("========================\n");
    printf("Reference:    %.3f seconds (sequential)\n", ref_time);
    printf("Critical:     %.3f seconds (%.1fx %s)\n", critical_time, 
           ref_time/critical_time, critical_time < ref_time ? "faster" : "slower");
    printf("Race:         %.3f seconds (%.1fx %s) %s\n", race_time, 
           ref_time/race_time, race_time < ref_time ? "faster" : "slower",
           fabs(pi_race - reference_pi) > 1e-10 ? "💥 WRONG!" : "");
    printf("No Sync:      %.3f seconds (%.1fx %s)\n", no_sync_time, 
           ref_time/no_sync_time, no_sync_time < ref_time ? "faster" : "slower");
    printf("False Fix:    %.3f seconds (%.1fx %s)\n", false_fix_time, 
           ref_time/false_fix_time, false_fix_time < ref_time ? "faster" : "slower");
    printf("Reduction:    %.3f seconds (%.1fx %s)\n", reduction_time, 
           ref_time/reduction_time, reduction_time < ref_time ? "faster" : "slower");
    printf("Atomic:       %.3f seconds (%.1fx %s)\n", atomic_time, 
           ref_time/atomic_time, atomic_time < ref_time ? "faster" : "slower");
    
    // Accuracy comparison
    printf("\n🎯 Accuracy Comparison\n");
    printf("=====================\n");
    printf("Reference:    %.15f (100.0000%% accurate)\n", reference_pi);
    printf("Critical:     %.15f (%.4f%% error)\n", pi_critical, 
           fabs(pi_critical - reference_pi) / reference_pi * 100);
    printf("Race:         %.15f (%.4f%% error) %s\n", pi_race, 
           fabs(pi_race - reference_pi) / reference_pi * 100,
           fabs(pi_race - reference_pi) > 1e-10 ? "💥 RACE CONDITION!" : "✅");
    printf("No Sync:      %.15f (%.4f%% error)\n", pi_no_sync, 
           fabs(pi_no_sync - reference_pi) / reference_pi * 100);
    printf("False Fix:    %.15f (%.4f%% error)\n", pi_false_fix, 
           fabs(pi_false_fix - reference_pi) / reference_pi * 100);
    printf("Reduction:    %.15f (%.4f%% error)\n", pi_reduction_result, 
           fabs(pi_reduction_result - reference_pi) / reference_pi * 100);
    printf("Atomic:       %.15f (%.4f%% error)\n", pi_atomic_result, 
           fabs(pi_atomic_result - reference_pi) / reference_pi * 100);
    
    printf("\n🎓 Key Lessons from Day2 Integration:\n");
    printf("- Critical sections work but have high overhead\n");
    printf("- Race conditions cause incorrect results\n");
    printf("- Proper thread-local storage avoids synchronization\n");
    printf("- False sharing can hurt performance even without races\n");
    printf("- Reduction is the most elegant solution\n");
    printf("- Atomic operations are fast but limited\n");
    
    printf("\n✅ Integration synchronization comparison completed!\n");
    return 0;
}
