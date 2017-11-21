CC = gcc 
CFLAGS = -Wall -std=c99
VPAHT = $(shell pwd)

objects = pc_web_s.o common_pc.o

all : $(objects)
	$(CC) $(objects) -o pc_web_s -pthread
pc_web_s.o : pc_web_s.c

.PHONNY : clean
clean:
	rm pc_web_s pc_web_s.o
