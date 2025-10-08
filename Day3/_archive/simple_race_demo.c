// Simple Race Condition Demo - Read-Modify-Write
// Compile: clang -fopenmp -O2 simple_race_demo.c -o simple_race_demo
// Run: OMP_NUM_THREADS=8 ./simple_race_demo

#include <omp.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#define NUM_THREADS 8
#define ITERATIONS 100000

// Global shared counter
static int shared_counter = 0;

// Unsafe increment - classic race condition
void unsafe_increment(int thread_id) {
    for (int i = 0; i < ITERATIONS; i++) {
        // This is the classic race condition:
        // 1. Read shared_counter
        // 2. Add 1 to it
        // 3. Write it back
        // Between steps 1 and 3, another thread can modify shared_counter!
        shared_counter = shared_counter + 1;
    }
    printf("🧵 Thread %d finished unsafe operations\n", thread_id);
}

// Safe increment using atomic
void safe_increment_atomic(int thread_id) {
    for (int i = 0; i < ITERATIONS; i++) {
        #pragma omp atomic
        shared_counter++;
    }
    printf("✅ Thread %d finished safe atomic operations\n", thread_id);
}

// Safe increment using critical section
void safe_increment_critical(int thread_id) {
    for (int i = 0; i < ITERATIONS; i++) {
        #pragma omp critical
        shared_counter++;
    }
    printf("✅ Thread %d finished safe critical operations\n", thread_id);
}

int main() {
    printf("🎬 Simple Race Condition Demo\n");
    printf("============================\n\n");
    
    printf("Running with %d threads, %d iterations each\n", NUM_THREADS, ITERATIONS);
    printf("Expected final counter value: %d\n\n", NUM_THREADS * ITERATIONS);
    
    // Test 1: Unsafe increment (race condition)
    printf("💥 Test 1: Unsafe Increment (Race Condition)\n");
    printf("============================================\n");
    shared_counter = 0;
    
    double start = omp_get_wtime();
    #pragma omp parallel num_threads(NUM_THREADS)
    {
        int thread_id = omp_get_thread_num();
        unsafe_increment(thread_id);
    }
    double unsafe_time = omp_get_wtime() - start;
    
    printf("Unsafe time: %.3f seconds\n", unsafe_time);
    printf("Final counter value: %d (Expected: %d)\n", 
           shared_counter, NUM_THREADS * ITERATIONS);
    printf("Lost updates: %d\n", (NUM_THREADS * ITERATIONS) - shared_counter);
    printf("Accuracy: %.1f%%\n\n", 
           (double)shared_counter / (NUM_THREADS * ITERATIONS) * 100);
    
    // Test 2: Safe increment with atomic
    printf("✅ Test 2: Safe Increment with Atomic\n");
    printf("====================================\n");
    shared_counter = 0;
    
    start = omp_get_wtime();
    #pragma omp parallel num_threads(NUM_THREADS)
    {
        int thread_id = omp_get_thread_num();
        safe_increment_atomic(thread_id);
    }
    double atomic_time = omp_get_wtime() - start;
    
    printf("Atomic time: %.3f seconds\n", atomic_time);
    printf("Final counter value: %d (Expected: %d)\n", 
           shared_counter, NUM_THREADS * ITERATIONS);
    printf("Correct: %s\n\n", shared_counter == NUM_THREADS * ITERATIONS ? "YES" : "NO");
    
    // Test 3: Safe increment with critical section
    printf("✅ Test 3: Safe Increment with Critical Section\n");
    printf("==============================================\n");
    shared_counter = 0;
    
    start = omp_get_wtime();
    #pragma omp parallel num_threads(NUM_THREADS)
    {
        int thread_id = omp_get_thread_num();
        safe_increment_critical(thread_id);
    }
    double critical_time = omp_get_wtime() - start;
    
    printf("Critical time: %.3f seconds\n", critical_time);
    printf("Final counter value: %d (Expected: %d)\n", 
           shared_counter, NUM_THREADS * ITERATIONS);
    printf("Correct: %s\n\n", shared_counter == NUM_THREADS * ITERATIONS ? "YES" : "NO");
    
    // Performance comparison
    printf("📊 Performance Comparison\n");
    printf("========================\n");
    printf("Unsafe:    %.3f seconds (baseline)\n", unsafe_time);
    printf("Atomic:    %.3f seconds (%.1fx %s)\n", atomic_time, 
           unsafe_time/atomic_time, atomic_time < unsafe_time ? "faster" : "slower");
    printf("Critical:  %.3f seconds (%.1fx %s)\n", critical_time, 
           unsafe_time/critical_time, critical_time < unsafe_time ? "faster" : "slower");
    
    printf("\n🎯 Why the Race Condition Happens:\n");
    printf("1. Thread A reads shared_counter (e.g., value = 100)\n");
    printf("2. Thread B reads shared_counter (e.g., value = 100)\n");
    printf("3. Thread A adds 1 and writes back (shared_counter = 101)\n");
    printf("4. Thread B adds 1 and writes back (shared_counter = 101)\n");
    printf("5. Result: Only 1 increment instead of 2!\n");
    
    printf("\n💡 The Fix:\n");
    printf("- Atomic operations: Hardware-level atomic increment\n");
    printf("- Critical sections: Only one thread at a time\n");
    printf("- Locks: Manual synchronization control\n");
    
    printf("\n✅ Simple race condition demo completed!\n");
    return 0;
}
