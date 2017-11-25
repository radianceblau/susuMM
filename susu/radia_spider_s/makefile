CC = arm-linux-gcc 
CFLAGS = -Wall -std=c99
VPAHT = $(shell pwd)

objects = radia_spider_s.o common_arm.o

all : $(objects)
	$(CC) $(objects) -o radia_spider_s -pthread
radia_spider_s.o : radia_spider_s.c

.PHONNY : clean
clean:
	rm radia_spider_s radia_spider_s.o
