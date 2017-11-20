CC = arm-linux-gcc 
CFLAGS = -Wall -std=c99
VPAHT = $(shell pwd)

objects = radia_display_s.o

radia_display_s : $(objects)
	$(CC) $(objects) -o radia_display_s -pthread

radia_display_s.o : radia_display_s.c

.PHONNY : clean
clean:
	rm radia_display_s $(objects)
