CC = gcc 
CFLAGS = -Wall -std=c99
VPAHT = $(shell pwd)

objects = pc_display_s.o

pc_display_s : $(objects)
	$(CC) $(objects) -o pc_display_s -pthread

pc_display_s.o : pc_display_s.c

.PHONNY : clean
clean:
	rm pc_display_s $(objects)
