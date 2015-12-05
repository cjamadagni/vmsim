# simple Makefile.

# the following .Phony means execute make clean or make depend even 
#if there are files named 'depend' and 'clean' in the directory
.PHONY: depend clean   

MAIN=vmsim
CC = gcc
CFLAGS= -DUNIX -g -Wall
INCLUDES = -I.

SRCS = fault.c	options.c  physmem.c  stats.c util.c	\
       pagetable.c  vmsim.c

OBJS = $(SRCS:.c=.o)

$(MAIN):  $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) -o $(MAIN) 

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

all: $(MAIN)

depend: $(SRCS)
	makedepend $(INCLUDES) $^

clean:
	@rm -f *.o *~ $(MAIN)

run:
	@./vmsim

