CC = gcc 
CFLAGS = -Wall -std=c99
VPAHT = $(shell pwd)

objects = pc_main_s.o

pc_main_s : $(objects)
	$(CC) $(objects) -o pc_main_s -D_SVID_SOURCE

pc_main_s.o : pc_main_s.c

.PHONNY : clean
clean:
	rm pc_main_s $(objects)
