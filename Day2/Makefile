CC = gcc
CLFAGS = -Wall -fopenmp
RM = rm -f

TARGETS = critical_section worksharing

all : $(TARGETS)
.PHONY : all

critical_section : critical_section.c
	$(CC) $(CLFAGS) critical_section.c -o critical_section

worksharing : worksharing.c
	$(CC) $(CLFAGS) worksharing.c -o worksharing

clean :
	$(RM) $(TARGETS)
.PHONY : clean