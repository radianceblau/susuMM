CC = gcc 
CFLAGS = -Wall -std=c99
VPAHT = $(shell pwd)

objects = pc_socket_test.o common_pc.o

all : $(objects)
	$(CC) $(objects) -o pc_socket_test
pc_socket_test.o : pc_socket_test.c

.PHONNY : clean
clean:
	rm pc_socket_test pc_socket_test.o