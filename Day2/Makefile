CC = gcc
CLFAGS = -Wall -fopenmp
RM = rm -f

TARGETS = critical_section worksharing int_sync int_nosync int_critical int_falsefix

all : $(TARGETS)
.PHONY : all

critical_section : critical_section.c
	$(CC) $(CLFAGS) critical_section.c -o critical_section

worksharing : worksharing.c
	$(CC) $(CLFAGS) worksharing.c -o worksharing

int_sync : int_sync.c
	$(CC) $(CLFAGS) int_sync.c -o int_sync

int_critical : int_critical.c
	$(CC) $(CLFAGS) int_critical.c -o int_critical

int_nosync : int_nosync.c
	$(CC) $(CLFAGS) int_nosync.c -o int_nosync

int_falsefix : int_falsefix.c
	$(CC) $(CLFAGS) int_falsefix.c -o int_falsefix

clean :
	$(RM) $(TARGETS)
.PHONY : clean