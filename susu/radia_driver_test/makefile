CC = arm-linux-gcc 
CFLAGS = -Wall -std=c99
VPAHT = $(shell pwd)

objects = radia_driver_test.o

all : $(objects)
	$(CC) $(objects) -o radia_driver_test
radia_driver_test.o : radia_driver_test.c

.PHONNY : clean
clean:
	rm radia_driver_test radia_driver_test.o