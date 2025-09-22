CC = gcc
CFLAGS = -Wall -fopenmp
RM = rm -f

TARGETS = helloworld int_final int_par1 int_par2 int_start

all : $(TARGETS)
.PHONY : all

helloworld : helloworld.c
	$(CC) $(CFLAGS) helloworld.c -o helloworld

int_final : int_final.c
	$(CC) $(CFLAGS) int_final.c -o int_final

int_par1 : int_par1.c
	$(CC) $(CFLAGS) int_par1.c -o int_par1

int_par2 : int_par2.c
	$(CC) $(CFLAGS) int_par2.c -o int_par2

int_serial : int_start.c
	$(CC) $(CFLAGS) int_start.c -o int_start

clean :
	$(RM) $(TARGETS)
.PHONY : clean