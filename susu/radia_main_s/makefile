CC = arm-linux-gcc 
CFLAGS = -Wall -std=c99
VPAHT = $(shell pwd)

objects = radia_main_s.o common_arm.o

all : $(objects)
	$(CC) $(objects) -o radia_main_s -pthread
radia_main_s.o : radia_main_s.c

.PHONNY : clean
clean:
	rm radia_main_s radia_main_s.o
