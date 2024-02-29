OBJS	= aliases.o execute.o history.o main.o tools.o
SOURCE	= aliases.c execute.c history.c main.c tools.c
HEADER	= aliases.h execute.h history.h tools.h
OUT	= mysh
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = 

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

aliases.o: aliases.c
	$(CC) $(FLAGS) aliases.c 

execute.o: execute.c
	$(CC) $(FLAGS) execute.c 

history.o: history.c
	$(CC) $(FLAGS) history.c 

main.o: main.c
	$(CC) $(FLAGS) main.c 

tools.o: tools.c
	$(CC) $(FLAGS) tools.c 


clean:
	rm -f $(OBJS) $(OUT)
