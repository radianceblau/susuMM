CC = gcc 
CFLAGS = -Wall -std=c99
VPAHT = $(shell pwd)

objects = pc_spider_s.o

pc_spider_s : $(objects)
	$(CC) $(objects) -o pc_spider_s -D_SVID_SOURCE

pc_spider_s.o : pc_spider_s.c

.PHONNY : clean
clean:
	rm pc_spider_s $(objects)
