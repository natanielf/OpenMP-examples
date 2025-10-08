# Makefile for OpenMP Day3 Examples
# Build: make
# Clean: make clean
# Run demos: make run-demos

CC = clang
CFLAGS = -O2 -Wall -Wextra -std=c99 -fopenmp
LDFLAGS = 
RM = rm -f

# All C files in Day3 directory
C_FILES = how-many.c fibonacci_task_recursion_main.c riemann_sum_tasks_main.c concurrent_tasks_demo.c
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
	@echo "  clean            - Remove all executables"
	@echo "  help             - Show this help message"
	@echo ""
	@echo "Usage examples:"
	@echo "  make                    # Build everything"
	@echo "  make run-demos          # Run all demos"
	@echo "  make run-concurrent     # Run concurrent tasks demo"
	@echo "  make run-how-many       # Run task counting puzzle"

# Debug build
debug: CFLAGS += -g -DDEBUG
debug: all

# Release build (optimized)
release: CFLAGS += -O3 -DNDEBUG
release: all

.PHONY: all clean help debug release run-demos run-how-many run-fibonacci run-riemann run-concurrent