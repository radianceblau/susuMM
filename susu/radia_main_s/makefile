CC = gcc 
CFLAGS = -Wall -std=c99
VPAHT = $(shell pwd)

objects = radia_main_s.o

radia_main_s : $(objects)
	$(CC) $(objects) -o radia_main_s -D_SVID_SOURCE

radia_main_s.o : radia_main_s.c

.PHONNY : clean
clean:
	rm radia_main_s $(objects)
