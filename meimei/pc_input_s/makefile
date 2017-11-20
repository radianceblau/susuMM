CC = gcc 
CFLAGS = -Wall -std=c99
VPAHT = $(shell pwd)

objects = pc_input_s.o

pc_input_s : $(objects)
	$(CC) $(objects) -o pc_input_s -pthread

pc_input_s.o : pc_input_s.c

.PHONNY : clean
clean:
	rm pc_input_s $(objects)
