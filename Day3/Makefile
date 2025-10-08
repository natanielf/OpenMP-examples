# Makefile for OpenMP Day3 Examples
# Build: make
# Clean: make clean
# Run demos: make run-demos

CC = clang
CFLAGS = -O2 -Wall -Wextra -std=c99 -fopenmp
LDFLAGS = 
RM = rm -f

# All C files in Day3 directory
C_FILES = how-many.c fibonacci_task_recursion_main.c riemann_sum_tasks_main.c concurrent_tasks_demo.c nested_basic.c nested_modified.c flat_monte_carlo.c nested_monte_carlo.c
TARGETS = $(C_FILES:.c=)

# Default target
all: $(TARGETS)
.PHONY: all

# Individual targets
how-many: how-many.c
	$(CC) $(CFLAGS) how-many.c -o how-many $(LDFLAGS)
	@echo "✅ How-many demo built"

fibonacci_task_recursion: fibonacci_task_recursion_main.c fibonacci_task_recursion.c
	$(CC) $(CFLAGS) fibonacci_task_recursion_main.c fibonacci_task_recursion.c -o fibonacci_task_recursion $(LDFLAGS)
	@echo "✅ Fibonacci task recursion demo built"

riemann_sum_tasks: riemann_sum_tasks_main.c riemann_sum_tasks.c
	$(CC) $(CFLAGS) riemann_sum_tasks_main.c riemann_sum_tasks.c -o riemann_sum_tasks $(LDFLAGS)
	@echo "✅ Riemann sum tasks demo built"

concurrent_tasks_demo: concurrent_tasks_demo.c fibonacci_task_recursion.c riemann_sum_tasks.c
	$(CC) $(CFLAGS) concurrent_tasks_demo.c fibonacci_task_recursion.c riemann_sum_tasks.c -o concurrent_tasks_demo $(LDFLAGS)
	@echo "✅ Concurrent tasks demo built"

nested_basic: nested_basic.c
	$(CC) $(CFLAGS) nested_basic.c -o nested_basic $(LDFLAGS)
	@echo "✅ Nested basic demo built"

nested_modified: nested_modified.c
	$(CC) $(CFLAGS) nested_modified.c -o nested_modified $(LDFLAGS)
	@echo "✅ Nested modified demo built"

flat_monte_carlo: flat_monte_carlo.c
	$(CC) $(CFLAGS) flat_monte_carlo.c -o flat_monte_carlo $(LDFLAGS)
	@echo "✅ Flat Monte Carlo demo built"

nested_monte_carlo: nested_monte_carlo.c
	$(CC) $(CFLAGS) nested_monte_carlo.c -o nested_monte_carlo $(LDFLAGS)
	@echo "✅ Nested Monte Carlo demo built"

# Run individual demos
run-how-many: how-many
	@echo "🎬 Running How-Many Demo..."
	@echo "=========================="
	OMP_NUM_THREADS=8 ./how-many

run-fibonacci: fibonacci_task_recursion
	@echo "🎬 Running Fibonacci Task Recursion Demo..."
	@echo "=========================================="
	OMP_NUM_THREADS=8 ./fibonacci_task_recursion

run-riemann: riemann_sum_tasks
	@echo "🎬 Running Riemann Sum Tasks Demo..."
	@echo "===================================="
	OMP_NUM_THREADS=8 ./riemann_sum_tasks

run-concurrent: concurrent_tasks_demo
	@echo "🎬 Running Concurrent Tasks Demo..."
	@echo "=================================="
	OMP_NUM_THREADS=8 ./concurrent_tasks_demo

run-nested-basic: nested_basic
	@echo "🎬 Running Nested Basic Demo..."
	@echo "=============================="
	OMP_NUM_THREADS=8 ./nested_basic

run-nested-modified: nested_modified
	@echo "🎬 Running Nested Modified Demo..."
	@echo "================================="
	OMP_NUM_THREADS=8 ./nested_modified

run-flat-monte: flat_monte_carlo
	@echo "🎬 Running Flat Monte Carlo Demo..."
	@echo "=================================="
	OMP_NUM_THREADS=8 ./flat_monte_carlo 10000000

run-nested-monte: nested_monte_carlo
	@echo "🎬 Running Nested Monte Carlo Demo..."
	@echo "===================================="
	OMP_NUM_THREADS=8 ./nested_monte_carlo 10000000

# Run all demos in sequence
run-demos: $(TARGETS)
	@echo "🎬 Running All OpenMP Day3 Demos"
	@echo "==============================="
	@echo ""
	@make run-how-many
	@echo ""
	@make run-fibonacci
	@echo ""
	@make run-riemann
	@echo ""
	@make run-concurrent
	@echo ""
	@make run-nested-basic
	@echo ""
	@make run-nested-modified
	@echo ""
	@make run-flat-monte
	@echo ""
	@make run-nested-monte
	@echo ""
	@echo "🎉 All demos completed!"

# Clean build artifacts
clean:
	$(RM) $(TARGETS)
	@echo "🧹 Cleaned all executables"

# Show help
help:
	@echo "🎬 OpenMP Day3 Examples Makefile"
	@echo "================================"
	@echo ""
	@echo "Available targets:"
	@echo "  all              - Build all programs"
	@echo "  run-demos        - Run all demos"
	@echo "  run-how-many     - Run how-many task counting demo"
	@echo "  run-fibonacci    - Run Fibonacci task recursion demo"
	@echo "  run-riemann      - Run Riemann sum tasks demo"
	@echo "  run-concurrent   - Run concurrent tasks demo"
	@echo "  run-nested-basic - Run nested parallelism basic demo"
	@echo "  run-nested-modified - Run nested parallelism modified demo"
	@echo "  run-flat-monte   - Run flat Monte Carlo pi estimation"
	@echo "  run-nested-monte - Run nested Monte Carlo pi estimation"
	@echo "  clean            - Remove all executables"
	@echo "  help             - Show this help message"
	@echo ""
	@echo "Usage examples:"
	@echo "  make                    # Build everything"
	@echo "  make run-demos          # Run all demos"
	@echo "  make run-concurrent     # Run concurrent tasks demo"
	@echo "  make run-how-many       # Run task counting puzzle"
	@echo "  make run-nested-basic   # Run nested parallelism demo"
	@echo "  make run-flat-monte     # Run Monte Carlo pi estimation"

# Debug build
debug: CFLAGS += -g -DDEBUG
debug: all

# Release build (optimized)
release: CFLAGS += -O3 -DNDEBUG
release: all

.PHONY: all clean help debug release run-demos run-how-many run-fibonacci run-riemann run-concurrent run-nested-basic run-nested-modified run-flat-monte run-nested-monte