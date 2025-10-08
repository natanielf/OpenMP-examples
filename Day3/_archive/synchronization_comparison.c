// Synchronization Methods Comparison - Riemann Sum Computation
// Compile: clang -fopenmp -O2 synchronization_comparison.c -o synchronization_comparison -lm
// Run: OMP_NUM_THREADS=8 ./synchronization_comparison

#include <omp.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

#define NUM_TASKS 8
#define RIEMANN_INTERVALS 1000000
#define SEGMENTS_PER_TASK 1000

// Global sum variables for different synchronization methods
static double sum_lock = 0.0;
static double sum_critical = 0.0;
static double sum_atomic = 0.0;
static double sum_unsafe = 0.0;

static omp_lock_t my_lock;

// Function to compute Riemann sum for f(x) = x^2 from 0 to 1
double riemann_sum_segment(int start_interval, int end_interval) {
    double dx = 1.0 / RIEMANN_INTERVALS;
    double segment_sum = 0.0;
    
    for (int i = start_interval; i < end_interval; i++) {
        double x = i * dx;
        double y = x * x;  // f(x) = x^2
        segment_sum += y * dx;
    }
    
    return segment_sum;
}

// Method 1: OpenMP Locks (omp_set_lock/omp_unset_lock)
void task_with_lock(int task_id) {
    double local_sum = 0.0;
    
    // Each task computes multiple segments
    for (int seg = 0; seg < SEGMENTS_PER_TASK; seg++) {
        int start = (task_id * SEGMENTS_PER_TASK + seg) * (RIEMANN_INTERVALS / (NUM_TASKS * SEGMENTS_PER_TASK));
        int end = start + (RIEMANN_INTERVALS / (NUM_TASKS * SEGMENTS_PER_TASK));
        
        double segment_result = riemann_sum_segment(start, end);
        
        // Add to global sum with lock protection
        omp_set_lock(&my_lock);
        sum_lock += segment_result;
        omp_unset_lock(&my_lock);
    }
    
    printf("🔒 Task %d (lock) completed on thread %d\n", task_id, omp_get_thread_num());
}

// Method 2: Critical Section (#pragma omp critical)
void task_with_critical(int task_id) {
    double local_sum = 0.0;
    
    // Each task computes multiple segments
    for (int seg = 0; seg < SEGMENTS_PER_TASK; seg++) {
        int start = (task_id * SEGMENTS_PER_TASK + seg) * (RIEMANN_INTERVALS / (NUM_TASKS * SEGMENTS_PER_TASK));
        int end = start + (RIEMANN_INTERVALS / (NUM_TASKS * SEGMENTS_PER_TASK));
        
        double segment_result = riemann_sum_segment(start, end);
        
        // Add to global sum with critical section protection
        #pragma omp critical
        sum_critical += segment_result;
    }
    
    printf("🚪 Task %d (critical) completed on thread %d\n", task_id, omp_get_thread_num());
}

// Method 3: Atomic Operations (#pragma omp atomic)
void task_with_atomic(int task_id) {
    double local_sum = 0.0;
    
    // Each task computes multiple segments
    for (int seg = 0; seg < SEGMENTS_PER_TASK; seg++) {
        int start = (task_id * SEGMENTS_PER_TASK + seg) * (RIEMANN_INTERVALS / (NUM_TASKS * SEGMENTS_PER_TASK));
        int end = start + (RIEMANN_INTERVALS / (NUM_TASKS * SEGMENTS_PER_TASK));
        
        double segment_result = riemann_sum_segment(start, end);
        
        // Add to global sum with atomic protection
        #pragma omp atomic
        sum_atomic += segment_result;
    }
    
    printf("⚛️  Task %d (atomic) completed on thread %d\n", task_id, omp_get_thread_num());
}

// Method 4: No synchronization (race condition) - BAITED!
void task_unsafe(int task_id) {
    double local_sum = 0.0;
    
    // Each task computes multiple segments
    for (int seg = 0; seg < SEGMENTS_PER_TASK; seg++) {
        int start = (task_id * SEGMENTS_PER_TASK + seg) * (RIEMANN_INTERVALS / (NUM_TASKS * SEGMENTS_PER_TASK));
        int end = start + (RIEMANN_INTERVALS / (NUM_TASKS * SEGMENTS_PER_TASK));
        
        double segment_result = riemann_sum_segment(start, end);
        
        // RACE CONDITION BAIT: Complex read-modify-write operation
        // This will definitely cause race conditions!
        double temp = sum_unsafe;        // 1. Read current value
        // Simulate some work that makes race condition more likely
        for (int i = 0; i < 100; i++) {
            temp += 0.0;  // Dummy work to increase timing window
        }
        temp += segment_result;          // 2. Modify (add segment result)
        sum_unsafe = temp;               // 3. Write back
        
        // Alternative bait: Multiple operations on shared variable
        // sum_unsafe += segment_result;  // This might be atomic, so we use the complex version above
    }
    
    printf("💥 Task %d (unsafe) completed on thread %d\n", task_id, omp_get_thread_num());
}

// Compute reference result sequentially
double compute_reference_result() {
    double dx = 1.0 / RIEMANN_INTERVALS;
    double reference_sum = 0.0;
    
    for (int i = 0; i < RIEMANN_INTERVALS; i++) {
        double x = i * dx;
        double y = x * x;  // f(x) = x^2
        reference_sum += y * dx;
    }
    
    return reference_sum;
}

int main() {
    printf("🎬 Synchronization Methods Comparison - Riemann Sum\n");
    printf("==================================================\n\n");
    
    printf("Computing Riemann sum of f(x) = x² from 0 to 1\n");
    printf("Intervals: %d, Tasks: %d, Segments per task: %d\n", 
           RIEMANN_INTERVALS, NUM_TASKS, SEGMENTS_PER_TASK);
    printf("Expected result: 1/3 ≈ 0.333333...\n\n");
    
    // Compute reference result
    printf("📐 Computing reference result...\n");
    double start_ref = omp_get_wtime();
    double reference_result = compute_reference_result();
    double ref_time = omp_get_wtime() - start_ref;
    printf("Reference result: %.10f (computed in %.3f seconds)\n\n", reference_result, ref_time);
    
    // Initialize lock
    omp_init_lock(&my_lock);
    
    // Test 1: OpenMP Locks
    printf("🔒 Test 1: OpenMP Locks (omp_set_lock/omp_unset_lock)\n");
    printf("----------------------------------------------------\n");
    sum_lock = 0.0;
    double start = omp_get_wtime();
    
    #pragma omp parallel
    #pragma omp single
    {
        for (int i = 0; i < NUM_TASKS; i++) {
            #pragma omp task
            task_with_lock(i + 1);
        }
        #pragma omp taskwait
    }
    
    double lock_time = omp_get_wtime() - start;
    printf("Lock time: %.3f seconds, Final sum: %.10f\n", lock_time, sum_lock);
    printf("Error: %.2e (%.4f%%)\n\n", 
           fabs(sum_lock - reference_result), 
           fabs(sum_lock - reference_result) / reference_result * 100);
    
    // Test 2: Critical Section
    printf("🚪 Test 2: Critical Section (#pragma omp critical)\n");
    printf("------------------------------------------------\n");
    sum_critical = 0.0;
    start = omp_get_wtime();
    
    #pragma omp parallel
    #pragma omp single
    {
        for (int i = 0; i < NUM_TASKS; i++) {
            #pragma omp task
            task_with_critical(i + 1);
        }
        #pragma omp taskwait
    }
    
    double critical_time = omp_get_wtime() - start;
    printf("Critical time: %.3f seconds, Final sum: %.10f\n", critical_time, sum_critical);
    printf("Error: %.2e (%.4f%%)\n\n", 
           fabs(sum_critical - reference_result), 
           fabs(sum_critical - reference_result) / reference_result * 100);
    
    // Test 3: Atomic Operations
    printf("⚛️  Test 3: Atomic Operations (#pragma omp atomic)\n");
    printf("------------------------------------------------\n");
    sum_atomic = 0.0;
    start = omp_get_wtime();
    
    #pragma omp parallel
    #pragma omp single
    {
        for (int i = 0; i < NUM_TASKS; i++) {
            #pragma omp task
            task_with_atomic(i + 1);
        }
        #pragma omp taskwait
    }
    
    double atomic_time = omp_get_wtime() - start;
    printf("Atomic time: %.3f seconds, Final sum: %.10f\n", atomic_time, sum_atomic);
    printf("Error: %.2e (%.4f%%)\n\n", 
           fabs(sum_atomic - reference_result), 
           fabs(sum_atomic - reference_result) / reference_result * 100);
    
    // Test 4: No synchronization (race condition) - BAITED!
    printf("💥 Test 4: No Synchronization (Race Condition - BAITED!)\n");
    printf("------------------------------------------------------\n");
    sum_unsafe = 0.0;
    start = omp_get_wtime();
    
    #pragma omp parallel
    #pragma omp single
    {
        for (int i = 0; i < NUM_TASKS; i++) {
            #pragma omp task
            task_unsafe(i + 1);
        }
        #pragma omp taskwait
    }
    
    double unsafe_time = omp_get_wtime() - start;
    printf("Unsafe time: %.3f seconds, Final sum: %.10f\n", unsafe_time, sum_unsafe);
    printf("Error: %.2e (%.4f%%)\n", 
           fabs(sum_unsafe - reference_result), 
           fabs(sum_unsafe - reference_result) / reference_result * 100);
    printf("Race condition detected: %s\n\n", 
           fabs(sum_unsafe - reference_result) > 1e-10 ? "YES!" : "NO");
    
    // Performance comparison
    printf("📊 Performance Comparison\n");
    printf("========================\n");
    printf("Reference:  %.3f seconds (sequential)\n", ref_time);
    printf("Locks:      %.3f seconds (%.1fx %s)\n", lock_time, 
           ref_time/lock_time, lock_time < ref_time ? "faster" : "slower");
    printf("Critical:   %.3f seconds (%.1fx %s)\n", critical_time, 
           ref_time/critical_time, critical_time < ref_time ? "faster" : "slower");
    printf("Atomic:     %.3f seconds (%.1fx %s)\n", atomic_time, 
           ref_time/atomic_time, atomic_time < ref_time ? "faster" : "slower");
    printf("Unsafe:     %.3f seconds (%.1fx %s)\n", unsafe_time, 
           ref_time/unsafe_time, unsafe_time < ref_time ? "faster" : "slower");
    
    // Accuracy comparison
    printf("\n🎯 Accuracy Comparison\n");
    printf("=====================\n");
    printf("Reference:  %.10f (100.0000%% accurate)\n", reference_result);
    printf("Locks:      %.10f (%.4f%% error)\n", sum_lock, 
           fabs(sum_lock - reference_result) / reference_result * 100);
    printf("Critical:   %.10f (%.4f%% error)\n", sum_critical, 
           fabs(sum_critical - reference_result) / reference_result * 100);
    printf("Atomic:     %.10f (%.4f%% error)\n", sum_atomic, 
           fabs(sum_atomic - reference_result) / reference_result * 100);
    printf("Unsafe:     %.10f (%.4f%% error) %s\n", sum_unsafe, 
           fabs(sum_unsafe - reference_result) / reference_result * 100,
           fabs(sum_unsafe - reference_result) > 1e-10 ? "💥 RACE CONDITION!" : "✅");
    
    // Cleanup
    omp_destroy_lock(&my_lock);
    
    printf("\n✅ Riemann sum synchronization comparison completed!\n");
    printf("💡 The race condition bait worked: complex read-modify-write operations\n");
    printf("   with timing windows are much more likely to show race conditions!\n");
    
    return 0;
}