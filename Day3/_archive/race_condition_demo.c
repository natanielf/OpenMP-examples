// Race Condition Demonstration
// Compile: clang -fopenmp -O2 race_condition_demo.c -o race_condition_demo
// Run: OMP_NUM_THREADS=8 ./race_condition_demo

#include <omp.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#define NUM_THREADS 8
#define ITERATIONS 1000000

// Global shared variables
static int shared_counter = 0;
static int shared_array[1000] = {0};
static int array_index = 0;

// Function to demonstrate race condition with counter
void race_counter_task(int thread_id) {
    for (int i = 0; i < ITERATIONS; i++) {
        // This is a classic race condition:
        // 1. Read counter value
        // 2. Increment it
        // 3. Write it back
        // Between steps 1 and 3, another thread can modify counter!
        int temp = shared_counter;
        temp = temp + 1;
        shared_counter = temp;
    }
    printf("🧵 Thread %d finished counter operations\n", thread_id);
}

// Function to demonstrate race condition with array access
void race_array_task(int thread_id) {
    for (int i = 0; i < ITERATIONS / 100; i++) {
        // Race condition: multiple threads accessing same array index
        int current_index = array_index;
        // Simulate some work
        for (int j = 0; j < 100; j++) {
            shared_array[current_index] += thread_id;
        }
        array_index = current_index + 1;
    }
    printf("🧵 Thread %d finished array operations\n", thread_id);
}

// Safe version using atomic operations
void safe_counter_task(int thread_id) {
    for (int i = 0; i < ITERATIONS; i++) {
        #pragma omp atomic
        shared_counter++;
    }
    printf("✅ Thread %d finished safe counter operations\n", thread_id);
}

// Safe version using critical section
void safe_array_task(int thread_id) {
    for (int i = 0; i < ITERATIONS / 100; i++) {
        #pragma omp critical
        {
            int current_index = array_index;
            for (int j = 0; j < 100; j++) {
                shared_array[current_index] += thread_id;
            }
            array_index = current_index + 1;
        }
    }
    printf("✅ Thread %d finished safe array operations\n", thread_id);
}

int main() {
    printf("🎬 Race Condition Demonstration\n");
    printf("==============================\n\n");
    
    printf("Running with %d threads, %d iterations each\n", NUM_THREADS, ITERATIONS);
    printf("Expected final counter value: %d\n\n", NUM_THREADS * ITERATIONS);
    
    // Test 1: Race condition with counter
    printf("💥 Test 1: Race Condition with Counter\n");
    printf("=====================================\n");
    shared_counter = 0;
    
    double start = omp_get_wtime();
    #pragma omp parallel num_threads(NUM_THREADS)
    {
        int thread_id = omp_get_thread_num();
        race_counter_task(thread_id);
    }
    double race_time = omp_get_wtime() - start;
    
    printf("Race condition time: %.3f seconds\n", race_time);
    printf("Final counter value: %d (Expected: %d)\n", 
           shared_counter, NUM_THREADS * ITERATIONS);
    printf("Lost updates: %d\n\n", (NUM_THREADS * ITERATIONS) - shared_counter);
    
    // Test 2: Safe version with atomic
    printf("✅ Test 2: Safe Counter with Atomic Operations\n");
    printf("=============================================\n");
    shared_counter = 0;
    
    start = omp_get_wtime();
    #pragma omp parallel num_threads(NUM_THREADS)
    {
        int thread_id = omp_get_thread_num();
        safe_counter_task(thread_id);
    }
    double safe_time = omp_get_wtime() - start;
    
    printf("Safe atomic time: %.3f seconds\n", safe_time);
    printf("Final counter value: %d (Expected: %d)\n", 
           shared_counter, NUM_THREADS * ITERATIONS);
    printf("Correct: %s\n\n", shared_counter == NUM_THREADS * ITERATIONS ? "YES" : "NO");
    
    // Test 3: Race condition with array
    printf("💥 Test 3: Race Condition with Array Access\n");
    printf("==========================================\n");
    
    // Reset array
    for (int i = 0; i < 1000; i++) {
        shared_array[i] = 0;
    }
    array_index = 0;
    
    start = omp_get_wtime();
    #pragma omp parallel num_threads(NUM_THREADS)
    {
        int thread_id = omp_get_thread_num();
        race_array_task(thread_id);
    }
    double race_array_time = omp_get_wtime() - start;
    
    printf("Race condition array time: %.3f seconds\n", race_array_time);
    printf("Final array index: %d\n", array_index);
    printf("Array corruption detected: %s\n\n", 
           array_index != (NUM_THREADS * ITERATIONS / 100) ? "YES" : "NO");
    
    // Test 4: Safe version with critical section
    printf("✅ Test 4: Safe Array Access with Critical Section\n");
    printf("=================================================\n");
    
    // Reset array
    for (int i = 0; i < 1000; i++) {
        shared_array[i] = 0;
    }
    array_index = 0;
    
    start = omp_get_wtime();
    #pragma omp parallel num_threads(NUM_THREADS)
    {
        int thread_id = omp_get_thread_num();
        safe_array_task(thread_id);
    }
    double safe_array_time = omp_get_wtime() - start;
    
    printf("Safe critical time: %.3f seconds\n", safe_array_time);
    printf("Final array index: %d\n", array_index);
    printf("Correct: %s\n\n", 
           array_index == (NUM_THREADS * ITERATIONS / 100) ? "YES" : "NO");
    
    // Performance comparison
    printf("📊 Performance Comparison\n");
    printf("========================\n");
    printf("Counter - Race: %.3f seconds\n", race_time);
    printf("Counter - Safe: %.3f seconds (%.1fx %s)\n", safe_time, 
           race_time/safe_time, safe_time < race_time ? "faster" : "slower");
    printf("Array - Race:   %.3f seconds\n", race_array_time);
    printf("Array - Safe:   %.3f seconds (%.1fx %s)\n", safe_array_time, 
           race_array_time/safe_array_time, safe_array_time < race_array_time ? "faster" : "slower");
    
    printf("\n🎯 Key Observations:\n");
    printf("- Race conditions cause lost updates and data corruption\n");
    printf("- Safe synchronization ensures correct results\n");
    printf("- Performance impact varies by synchronization method\n");
    printf("- Race conditions are non-deterministic - results may vary!\n");
    
    printf("\n✅ Race condition demonstration completed!\n");
    return 0;
}
