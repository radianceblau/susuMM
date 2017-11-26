CC = arm-linux-gcc 
CFLAGS = -Wall -std=c99
VPAHT = $(shell pwd)

objects = radia_nl_test.o

all : $(objects)
	$(CC) $(objects) -o radia_nl_test
radia_nl_test.o : radia_nl_test.c

.PHONNY : clean
clean:
	rm radia_nl_test radia_nl_test.o