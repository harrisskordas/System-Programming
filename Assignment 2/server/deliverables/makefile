OBJS	= execute.o statistics.o main.o prod_cons.o tools.o votes.o
SOURCE	= execute.c statistics.c main.c prod_cons.c tools.c votes.c
HEADER	= execute.h statistics.h prod_cons.h tools.h votes.h
OUT	= poller
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = -lpthread

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

execute.o: execute.c
	$(CC) $(FLAGS) execute.c 

statistics.o: statistics.c
	$(CC) $(FLAGS) statistics.c

main.o: main.c
	$(CC) $(FLAGS) main.c 

prod_cons.o: prod_cons.c
	$(CC) $(FLAGS) prod_cons.c 

tools.o: tools.c
	$(CC) $(FLAGS) tools.c 

votes.o: votes.c
	$(CC) $(FLAGS) votes.c 


clean:
	rm -f $(OBJS) $(OUT)
