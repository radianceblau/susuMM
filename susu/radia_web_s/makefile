CC = arm-linux-gcc 
CFLAGS = -Wall -std=c99
VPAHT = $(shell pwd)

objects = radia_web_s.o common_arm.o

all : $(objects)
	$(CC) $(objects) -o radia_web_s -pthread
radia_web_s.o : radia_web_s.c

.PHONNY : clean
clean:
	rm radia_web_s radia_web_s.o
