CC = gcc
CFLAGS = -Wall -fopenmp
RM = rm -f

TARGETS = helloworld int_final int_par1 int_par2 int_start int_test

all : $(TARGETS)
.PHONY : all

webserver : webserver.c
	$(CC) $(CFLAGS) webserver.c -o webserver

how-many : how-many.c
	$(CC) $(CFLAGS) how-many.c -o how-many

clean :
	$(RM) $(TARGETS)
.PHONY : clean