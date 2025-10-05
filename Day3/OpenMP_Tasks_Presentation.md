# OpenMP Tasks: From Fundamentals to Advanced Applications

*A Comprehensive Guide to Parallel Task Management*

---

## Slide 1: Title Slide

# OpenMP Tasks
## From Fundamentals to Advanced Applications

**Topics Covered:**
- Task Fundamentals
- Task Synchronization
- Task Dependencies
- Nested Tasks
- Real-World Applications

---

## Slide 2: What Are OpenMP Tasks?

### Definition
- **Tasks** are units of work that can be executed asynchronously
- Unlike parallel loops, tasks represent **irregular parallelism**
- Tasks can be created **dynamically** at runtime
- Tasks are executed by **available threads** in the team

### Key Characteristics
- **Asynchronous execution**
- **Dynamic creation**
- **Irregular work patterns**
- **Load balancing** by OpenMP runtime

---

## Slide 3: When to Use Tasks

### Traditional Parallel Constructs
```c
#pragma omp parallel for
for (int i = 0; i < n; i++) {
    // Regular, predictable work
}
```

### Task-Based Parallelism
```c
#pragma omp parallel
{
    #pragma omp single
    {
        for (int i = 0; i < n; i++) {
            #pragma omp task
            {
                // Irregular, unpredictable work
            }
        }
    }
}
```

### Use Cases
- **Recursive algorithms**
- **Dynamic work generation**
- **Event-driven programming**
- **Irregular data structures**

---

## Slide 4: Basic Task Syntax

### Simple Task Creation
```c
#pragma omp task
{
    // Task body - executed by any available thread
    printf("Task executed by thread %d\n", omp_get_thread_num());
}
```

### Task with Data Clauses
```c
int shared_var = 0;
int private_var = 42;

#pragma omp task shared(shared_var) firstprivate(private_var)
{
    // shared_var is shared among all tasks
    // private_var is private to this task
    shared_var += private_var;
}
```

---

## Slide 5: Task Data Clauses

### Available Clauses
- **`shared(variables)`** - Shared among all tasks
- **`private(variables)`** - Private to each task
- **`firstprivate(variables)`** - Private with initialization
- **`lastprivate(variables)`** - Private with final value
- **`default(shared|private|none)`** - Default data sharing

### Example
```c
int a = 1, b = 2, c = 3;

#pragma omp task shared(a) firstprivate(b) private(c)
{
    // a is shared, b is copied, c is uninitialized
    a += b;  // OK: a is shared, b is initialized
    // c += 1;  // ERROR: c is uninitialized
}
```

---

## Slide 6: Task Synchronization

### `taskwait` Directive
```c
#pragma omp parallel
{
    #pragma omp single
    {
        for (int i = 0; i < 10; i++) {
            #pragma omp task
            {
                // Some work
            }
        }
        #pragma omp taskwait  // Wait for all tasks to complete
        printf("All tasks completed\n");
    }
}
```

### Purpose
- **Synchronization point** for task completion
- **Implicit barrier** for all tasks in the current context
- **Ensures** all tasks finish before continuing

---

## Slide 7: Task Dependencies

### `depend` Clause
```c
int result1, result2, final_result;

#pragma omp task depend(out: result1)
{
    result1 = compute_something();
}

#pragma omp task depend(out: result2)
{
    result2 = compute_something_else();
}

#pragma omp task depend(in: result1, result2)
{
    final_result = result1 + result2;
}
```

### Dependency Types
- **`depend(in: var)`** - Task depends on var being ready
- **`depend(out: var)`** - Task produces var
- **`depend(inout: var)`** - Task both reads and writes var

---

## Slide 8: Nested Tasks

### Basic Nesting
```c
#pragma omp parallel
{
    #pragma omp single
    {
        #pragma omp task
        {
            printf("Outer task: thread %d\n", omp_get_thread_num());
            
            #pragma omp task
            {
                printf("Inner task: thread %d\n", omp_get_thread_num());
            }
        }
    }
}
```

### Configuration
```c
omp_set_max_active_levels(2);  // Allow 2 levels of nesting
```

---

## Slide 9: Nested Parallelism

### Nested Parallel Regions
```c
#pragma omp parallel
{
    #pragma omp single
    {
        #pragma omp task
        {
            // Outer task
            #pragma omp parallel num_threads(2)
            {
                // Nested parallel region
                printf("Nested thread %d\n", omp_get_thread_num());
            }
        }
    }
}
```

### Thread Management
- **Outer level**: Uses main thread team
- **Inner level**: Creates new thread team
- **Total threads**: Can exceed `OMP_NUM_THREADS`

---

## Slide 10: Task Scheduling

### Scheduling Policies
- **`schedule(static)`** - Tasks assigned at creation time
- **`schedule(dynamic)`** - Tasks assigned when threads become available
- **`schedule(guided)`** - Adaptive scheduling
- **`schedule(runtime)`** - Use environment variable

### Example
```c
#pragma omp task schedule(dynamic)
{
    // Task with dynamic scheduling
}
```

---

## Slide 11: Task Groups

### `taskgroup` Directive
```c
#pragma omp taskgroup
{
    #pragma omp task
    {
        // Task 1
    }
    #pragma omp task
    {
        // Task 2
    }
    // Implicit taskwait at end of taskgroup
}
```

### Benefits
- **Scoped synchronization**
- **Better performance** than explicit taskwait
- **Cleaner code structure**

---

## Slide 12: Task Loops

### `taskloop` Directive
```c
#pragma omp taskloop
for (int i = 0; i < n; i++) {
    // Each iteration becomes a task
    process_item(i);
}
```

### With Clauses
```c
#pragma omp taskloop num_tasks(4) grainsize(10)
for (int i = 0; i < 100; i++) {
    // Creates 4 tasks with ~10 iterations each
}
```

---

## Slide 13: Performance Considerations

### Task Overhead
- **Creation cost**: Each task has overhead
- **Scheduling cost**: Runtime must manage task queue
- **Synchronization cost**: taskwait and barriers

### Optimization Strategies
- **Batch small tasks** together
- **Use taskloop** for regular patterns
- **Minimize taskwait** calls
- **Profile** to identify bottlenecks

---

## Slide 14: Common Patterns

### Producer-Consumer
```c
#pragma omp parallel
{
    #pragma omp single
    {
        // Producer
        for (int i = 0; i < n; i++) {
            #pragma omp task
            {
                // Consumer work
                process_item(i);
            }
        }
    }
}
```

### Recursive Algorithms
```c
void recursive_function(int n) {
    if (n <= 1) return;
    
    #pragma omp task
    recursive_function(n/2);
    
    #pragma omp task
    recursive_function(n/2);
    
    #pragma omp taskwait
}
```

---

## Slide 15: Error Handling

### Common Mistakes
```c
// WRONG: Race condition
int shared_var = 0;
#pragma omp task
{
    shared_var++;  // Race condition!
}

// CORRECT: Use atomic or critical
#pragma omp task
{
    #pragma omp atomic
    shared_var++;
}
```

### Best Practices
- **Use atomic operations** for simple updates
- **Use critical sections** for complex operations
- **Avoid shared mutable state**
- **Test with different thread counts**

---

## Slide 16: Debugging Tasks

### Debugging Tools
- **Thread sanitizer** (TSan)
- **OpenMP runtime** debugging
- **Print statements** with thread IDs
- **Timing measurements**

### Example
```c
#pragma omp task
{
    printf("Task %d on thread %d at time %f\n", 
           task_id, omp_get_thread_num(), omp_get_wtime());
}
```

---

## Slide 17: Real-World Applications

### Network Servers
- **Connection handling**
- **Request processing**
- **Load balancing**

### Scientific Computing
- **Tree traversals**
- **Graph algorithms**
- **Irregular meshes**

### Data Processing
- **Pipeline processing**
- **Event handling**
- **Stream processing**

---

## Slide 18: Advanced Features

### Task Affinity
```c
#pragma omp task affinity(iterator(i:0:n-1))
{
    // Task prefers to run on thread i
}
```

### Task Priority
```c
#pragma omp task priority(10)
{
    // Higher priority task
}
```

### Task Reduction
```c
int sum = 0;
#pragma omp taskloop reduction(+:sum)
for (int i = 0; i < n; i++) {
    sum += array[i];
}
```

---

## Slide 19: Best Practices

### Design Principles
1. **Minimize task creation overhead**
2. **Use appropriate data sharing**
3. **Balance task granularity**
4. **Avoid excessive synchronization**
5. **Profile and measure performance**

### Code Organization
- **Keep tasks focused** on single responsibilities
- **Use clear data dependencies**
- **Document task interactions**
- **Test with various thread counts**

---

## Slide 20: Summary

### Key Takeaways
- **Tasks enable irregular parallelism**
- **Dynamic work generation** at runtime
- **Flexible synchronization** with taskwait
- **Nested parallelism** for complex applications
- **Performance depends** on proper design

### Next Steps
- **Practice** with simple examples
- **Experiment** with different patterns
- **Profile** your applications
- **Explore** advanced features

---

## Slide 21: Resources

### Documentation
- **OpenMP 5.0 Specification**
- **OpenMP Runtime Library**
- **Compiler documentation**

### Tools
- **GCC OpenMP** implementation
- **Intel OpenMP** runtime
- **LLVM OpenMP** runtime

### Examples
- **OpenMP examples** repository
- **Scientific computing** libraries
- **Parallel algorithms** textbooks

---

## Slide 22: Questions & Discussion

### Common Questions
- How do I choose between tasks and parallel loops?
- What's the optimal task granularity?
- How do I debug task-related issues?
- When should I use nested parallelism?

### Hands-on Practice
- **Implement** a task-based algorithm
- **Compare** performance with other approaches
- **Experiment** with different configurations
- **Share** your experiences and challenges

---

*End of Presentation*

