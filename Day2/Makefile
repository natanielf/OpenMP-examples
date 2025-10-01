CC = gcc
CLFAGS = -Wall -fopenmp
RM = rm -f

TARGETS = critical_section worksharing int_sync

all : $(TARGETS)
.PHONY : all

critical_section : critical_section.c
	$(CC) $(CLFAGS) critical_section.c -o critical_section

worksharing : worksharing.c
	$(CC) $(CLFAGS) worksharing.c -o worksharing

int_sync : int_sync.c
	$(CC) $(CLFAGS) int_sync.c -o int_sync

clean :
	$(RM) $(TARGETS)
.PHONY : clean