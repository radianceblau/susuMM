CC = arm-linux-gcc 
CFLAGS = -Wall -std=c99
VPAHT = $(shell pwd)

objects = radia_input_s.o common_arm.o

all : $(objects)
	$(CC) $(objects) -o radia_input_s -pthread
radia_input_s.o : radia_input_s.c

.PHONNY : clean
clean:
	rm radia_input_s radia_input_s.o
