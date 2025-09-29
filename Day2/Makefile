CC = gcc
CLFAGS = -Wall -fopenmp
RM = rm -f

TARGETS = critical_section

all : $(TARGETS)
.PHONY : all

critical_section : critical_section.c
	$(CC) $(CLFAGS) critical_section.c -o critical_section

clean :
	$(RM) $(TARGETS)
.PHONY : clean