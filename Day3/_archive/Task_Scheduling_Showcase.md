# OpenMP Task Scheduling Showcase
## Interactive Lecture Demonstration

---

## 🎯 **Learning Objectives**
By the end of this showcase, you will understand:
- How OpenMP schedules tasks across threads
- The impact of task priorities and dependencies
- Real-time visualization of task execution
- Performance implications of different scheduling strategies

---

## 🚀 **Demo 1: Basic Task Scheduling**

### **The Setup**
```c
// basic_scheduling.c
#include <omp.h>
#include <stdio.h>
#include <unistd.h>

void worker_task(int task_id, int duration_ms) {
    printf("🟢 Task %d STARTED on thread %d\n", task_id, omp_get_thread_num());
    usleep(duration_ms * 1000);  // Simulate work
    printf("🔴 Task %d FINISHED on thread %d\n", task_id, omp_get_thread_num());
}

int main() {
    printf("🎬 Demo 1: Basic Task Scheduling\n");
    printf("Threads available: %d\n\n", omp_get_max_threads());
    
    #pragma omp parallel num_threads(3)
    #pragma omp single
    {
        for (int i = 1; i <= 6; i++) {
            #pragma omp task
            worker_task(i, 500);  // 500ms each
        }
        #pragma omp taskwait
    }
    
    printf("\n✅ All tasks completed!\n");
    return 0;
}
```

### **Expected Output Pattern**
```
🎬 Demo 1: Basic Task Scheduling
Threads available: 3

🟢 Task 1 STARTED on thread 0
🟢 Task 2 STARTED on thread 1  
🟢 Task 3 STARTED on thread 2
🔴 Task 1 FINISHED on thread 0
🟢 Task 4 STARTED on thread 0
🔴 Task 2 FINISHED on thread 1
🟢 Task 5 STARTED on thread 1
🔴 Task 3 FINISHED on thread 2
🟢 Task 6 STARTED on thread 2
🔴 Task 4 FINISHED on thread 0
🔴 Task 5 FINISHED on thread 1
🔴 Task 6 FINISHED on thread 2

✅ All tasks completed!
```

### **Key Observations**
- **Load Balancing**: OpenMP distributes tasks across available threads
- **Dynamic Scheduling**: Tasks are assigned as threads become available
- **No Guaranteed Order**: Tasks may complete in different order than created

---

## 🎯 **Demo 2: Task Priorities**

### **The Challenge**
What happens when tasks have different priorities?

```c
// priority_scheduling.c
#include <omp.h>
#include <stdio.h>
#include <unistd.h>

void priority_task(int task_id, int priority, int duration_ms) {
    printf("🟢 Task %d (priority %d) STARTED on thread %d\n", 
           task_id, priority, omp_get_thread_num());
    usleep(duration_ms * 1000);
    printf("🔴 Task %d (priority %d) FINISHED on thread %d\n", 
           task_id, priority, omp_get_thread_num());
}

int main() {
    printf("🎬 Demo 2: Task Priorities\n");
    printf("Higher numbers = higher priority\n\n");
    
    #pragma omp parallel num_threads(2)
    #pragma omp single
    {
        // Create tasks with different priorities
        #pragma omp task priority(1)
        priority_task(1, 1, 800);
        
        #pragma omp task priority(10)
        priority_task(2, 10, 600);
        
        #pragma omp task priority(5)
        priority_task(3, 5, 400);
        
        #pragma omp task priority(20)
        priority_task(4, 20, 300);
        
        #pragma omp taskwait
    }
    
    printf("\n✅ Priority demo completed!\n");
    return 0;
}
```

### **Expected Output Pattern**
```
🎬 Demo 2: Task Priorities
Higher numbers = higher priority

🟢 Task 2 (priority 10) STARTED on thread 0
🟢 Task 4 (priority 20) STARTED on thread 1
🔴 Task 4 (priority 20) FINISHED on thread 1
🟢 Task 3 (priority 5) STARTED on thread 1
🔴 Task 2 (priority 10) FINISHED on thread 0
🟢 Task 1 (priority 1) STARTED on thread 0
🔴 Task 3 (priority 5) FINISHED on thread 1
🔴 Task 1 (priority 1) FINISHED on thread 0

✅ Priority demo completed!
```

### **Key Observations**
- **Priority Scheduling**: Higher priority tasks tend to start first
- **Runtime Behavior**: Actual scheduling depends on OpenMP implementation
- **Thread Availability**: Priorities work within available thread constraints

---

## 🔗 **Demo 3: Task Dependencies**

### **The Dependency Chain**
```c
// dependency_scheduling.c
#include <omp.h>
#include <stdio.h>
#include <unistd.h>

int shared_data = 0;

void dependent_task(int task_id, int duration_ms) {
    printf("🟢 Task %d STARTED on thread %d (data=%d)\n", 
           task_id, omp_get_thread_num(), shared_data);
    usleep(duration_ms * 1000);
    shared_data += task_id;
    printf("🔴 Task %d FINISHED on thread %d (data=%d)\n", 
           task_id, omp_get_thread_num(), shared_data);
}

int main() {
    printf("🎬 Demo 3: Task Dependencies\n");
    printf("Tasks must wait for their dependencies!\n\n");
    
    #pragma omp parallel num_threads(3)
    #pragma omp single
    {
        // Task 1: Produces data
        #pragma omp task depend(out: shared_data)
        dependent_task(1, 500);
        
        // Task 2: Depends on Task 1
        #pragma omp task depend(in: shared_data)
        dependent_task(2, 300);
        
        // Task 3: Also depends on Task 1
        #pragma omp task depend(in: shared_data)
        dependent_task(3, 400);
        
        // Task 4: Depends on Task 2
        #pragma omp task depend(in: shared_data)
        dependent_task(4, 200);
        
        #pragma omp taskwait
    }
    
    printf("\n✅ Dependency demo completed!\n");
    return 0;
}
```

### **Expected Output Pattern**
```
🎬 Demo 3: Task Dependencies
Tasks must wait for their dependencies!

🟢 Task 1 STARTED on thread 0 (data=0)
🔴 Task 1 FINISHED on thread 0 (data=1)
🟢 Task 2 STARTED on thread 0 (data=1)
🟢 Task 3 STARTED on thread 1 (data=1)
🔴 Task 2 FINISHED on thread 0 (data=3)
🟢 Task 4 STARTED on thread 0 (data=3)
🔴 Task 3 FINISHED on thread 1 (data=6)
🔴 Task 4 FINISHED on thread 0 (data=10)

✅ Dependency demo completed!
```

### **Key Observations**
- **Dependency Enforcement**: Tasks wait for their dependencies
- **Parallel Execution**: Independent tasks can run simultaneously
- **Data Flow**: Dependencies create a directed acyclic graph (DAG)

---

## 🎭 **Demo 4: Priority Inversion Problem**

### **The Classic Problem**
```c
// priority_inversion.c
#include <omp.h>
#include <stdio.h>
#include <unistd.h>

int critical_resource = 0;

void high_priority_task() {
    printf("🔥 HIGH priority task STARTED on thread %d\n", omp_get_thread_num());
    usleep(200000);  // 200ms
    printf("🔥 HIGH priority task FINISHED on thread %d\n", omp_get_thread_num());
}

void low_priority_task() {
    printf("🐌 LOW priority task STARTED on thread %d\n", omp_get_thread_num());
    usleep(1000000);  // 1000ms - long task
    printf("🐌 LOW priority task FINISHED on thread %d\n", omp_get_thread_num());
}

void medium_priority_task() {
    printf("⚡ MEDIUM priority task STARTED on thread %d\n", omp_get_thread_num());
    usleep(300000);  // 300ms
    printf("⚡ MEDIUM priority task FINISHED on thread %d\n", omp_get_thread_num());
}

int main() {
    printf("🎬 Demo 4: Priority Inversion Problem\n");
    printf("Watch what happens with limited threads!\n\n");
    
    #pragma omp parallel num_threads(2)  // Limited threads!
    #pragma omp single
    {
        // Create tasks in priority order
        #pragma omp task priority(100) depend(out: critical_resource)
        high_priority_task();
        
        #pragma omp task priority(1) depend(in: critical_resource)
        low_priority_task();
        
        #pragma omp task priority(50)
        medium_priority_task();
        
        #pragma omp task priority(50)
        medium_priority_task();
        
        #pragma omp taskwait
    }
    
    printf("\n✅ Priority inversion demo completed!\n");
    return 0;
}
```

### **Expected Output Pattern**
```
🎬 Demo 4: Priority Inversion Problem
Watch what happens with limited threads!

🔥 HIGH priority task STARTED on thread 0
🐌 LOW priority task STARTED on thread 1
⚡ MEDIUM priority task STARTED on thread 0
🔴 HIGH priority task FINISHED on thread 0
⚡ MEDIUM priority task STARTED on thread 0
🔴 MEDIUM priority task FINISHED on thread 0
🔴 MEDIUM priority task FINISHED on thread 0
🔴 LOW priority task FINISHED on thread 1

✅ Priority inversion demo completed!
```

### **Key Observations**
- **Priority Inversion**: High priority task waits for low priority task
- **Thread Starvation**: Limited threads can cause scheduling issues
- **Dependency Impact**: Dependencies can override priority scheduling

---

## 📊 **Demo 5: Performance Comparison**

### **The Benchmark**
```c
// performance_comparison.c
#include <omp.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

void compute_task(int task_id, int iterations) {
    volatile int sum = 0;
    for (int i = 0; i < iterations; i++) {
        sum += i * task_id;
    }
}

int main() {
    const int num_tasks = 1000;
    const int iterations = 100000;
    
    printf("🎬 Demo 5: Performance Comparison\n");
    printf("Running %d tasks with %d iterations each\n\n", num_tasks, iterations);
    
    // Test 1: Sequential execution
    printf("📈 Test 1: Sequential Execution\n");
    double start = omp_get_wtime();
    for (int i = 0; i < num_tasks; i++) {
        compute_task(i, iterations);
    }
    double sequential_time = omp_get_wtime() - start;
    printf("Sequential time: %.3f seconds\n\n", sequential_time);
    
    // Test 2: Parallel tasks
    printf("📈 Test 2: Parallel Tasks\n");
    start = omp_get_wtime();
    #pragma omp parallel
    #pragma omp single
    {
        for (int i = 0; i < num_tasks; i++) {
            #pragma omp task
            compute_task(i, iterations);
        }
        #pragma omp taskwait
    }
    double parallel_time = omp_get_wtime() - start;
    printf("Parallel time: %.3f seconds\n\n", parallel_time);
    
    // Results
    double speedup = sequential_time / parallel_time;
    printf("🚀 Speedup: %.2fx\n", speedup);
    printf("📊 Efficiency: %.1f%%\n", (speedup / omp_get_max_threads()) * 100);
    
    return 0;
}
```

### **Expected Output Pattern**
```
🎬 Demo 5: Performance Comparison
Running 1000 tasks with 100000 iterations each

📈 Test 1: Sequential Execution
Sequential time: 2.456 seconds

📈 Test 2: Parallel Tasks
Parallel time: 0.623 seconds

🚀 Speedup: 3.94x
📊 Efficiency: 98.5%
```

---

## 🎮 **Interactive Demo: Real-Time Task Visualization**

### **The Live Showcase**
```c
// live_visualization.c
#include <omp.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

void animated_task(int task_id, int duration_ms) {
    printf("🎬 Task %d: ", task_id);
    fflush(stdout);
    
    for (int i = 0; i < 20; i++) {
        printf("█");
        fflush(stdout);
        usleep(duration_ms * 1000 / 20);
    }
    printf(" ✅ (thread %d)\n", omp_get_thread_num());
}

int main() {
    printf("🎮 LIVE DEMO: Real-Time Task Visualization\n");
    printf("Watch tasks execute in real-time!\n\n");
    
    #pragma omp parallel num_threads(4)
    #pragma omp single
    {
        for (int i = 1; i <= 8; i++) {
            #pragma omp task
            animated_task(i, 200);  // 200ms per task
        }
        #pragma omp taskwait
    }
    
    printf("\n🎉 All tasks completed!\n");
    return 0;
}
```

---

## 🎯 **Key Takeaways for Students**

### **1. Task Scheduling is Dynamic**
- OpenMP runtime makes scheduling decisions at runtime
- Tasks are assigned to threads as they become available
- No guaranteed execution order without dependencies

### **2. Priorities are Hints**
- Higher priority tasks tend to start first
- But thread availability and dependencies matter more
- Priorities work best with sufficient threads

### **3. Dependencies Override Priorities**
- Task dependencies create execution constraints
- A low-priority task can block high-priority tasks
- Design dependency graphs carefully

### **4. Performance Depends on Granularity**
- Too many small tasks = overhead
- Too few large tasks = poor load balancing
- Find the sweet spot for your workload

### **5. Thread Count Matters**
- More threads = better parallelism
- But also more overhead and contention
- Profile to find optimal thread count

---

## 🛠️ **Compilation and Running**

### **Build Commands**
```bash
# For GCC with OpenMP
gcc -fopenmp -O2 basic_scheduling.c -o basic_scheduling
gcc -fopenmp -O2 priority_scheduling.c -o priority_scheduling
gcc -fopenmp -O2 dependency_scheduling.c -o dependency_scheduling
gcc -fopenmp -O2 priority_inversion.c -o priority_inversion
gcc -fopenmp -O2 performance_comparison.c -o performance_comparison
gcc -fopenmp -O2 live_visualization.c -o live_visualization

# For Clang (if OpenMP is available)
clang -fopenmp -O2 basic_scheduling.c -o basic_scheduling
```

### **Running with Different Thread Counts**
```bash
# Test with different thread counts
OMP_NUM_THREADS=2 ./basic_scheduling
OMP_NUM_THREADS=4 ./basic_scheduling
OMP_NUM_THREADS=8 ./basic_scheduling
```

---

## 🎓 **Discussion Questions**

1. **Why might the actual execution order differ from the expected pattern?**
2. **How would you modify the priority inversion demo to avoid the problem?**
3. **What happens if you increase the number of threads in the dependency demo?**
4. **How does task granularity affect the performance results?**
5. **What real-world applications could benefit from task-based parallelism?**

---

## 🚀 **Next Steps**

- **Experiment** with different thread counts
- **Modify** task durations and priorities
- **Add** more complex dependency graphs
- **Profile** your own applications
- **Explore** advanced OpenMP features

---

*This showcase provides hands-on experience with OpenMP task scheduling concepts that are essential for understanding parallel programming and building high-performance applications.*
