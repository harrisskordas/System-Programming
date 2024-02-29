OBJS	= execute.o main.o tools.o
SOURCE	= execute.c main.c tools.c
HEADER	= execute.h tools.h
OUT	= pollSwayer
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = -lpthread

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

execute.o: execute.c
	$(CC) $(FLAGS) execute.c 

main.o: main.c
	$(CC) $(FLAGS) main.c 

tools.o: tools.c
	$(CC) $(FLAGS) tools.c 


clean:
	rm -f $(OBJS) $(OUT)
