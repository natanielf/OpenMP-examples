CC = gcc
CLFAGS = -Wall -fopenmp
RM = rm -f

TARGETS = hello_world loop_comparison matmul_benchmark critical_section

all : $(TARGETS)
.PHONY : all

hello_world : hello_world.c
	$(CC) $(CLFAGS) hello_world.c -o hello_world

loop_comparison : loop_comparison.c
	$(CC) $(CLFAGS) loop_comparison.c -o loop_comparison

matmul_benchmark : matmul_benchmark.c
	$(CC) $(CLFAGS) matmul_benchmark.c -o matmul_benchmark

critical_section : critical_section.c
	$(CC) $(CLFAGS) critical_section.c -o critical_section

clean :
	$(RM) $(TARGETS)
.PHONY : clean