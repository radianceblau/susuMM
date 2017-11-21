CC = gcc 
CFLAGS = -Wall -std=c99
VPAHT = $(shell pwd)

objects = common_arm.o common_pc.o
all : $(objects)

common_arm.o : common.c
	arm-linux-gcc -c common.c -o common_arm.o
	cp common_arm.o ../susu/radia_display_s
	cp common_arm.o ../susu/radia_input_s
	cp common_arm.o ../susu/radia_main_s
	cp common_arm.o ../susu/radia_web_s
common_pc.o : common.c
	gcc -c common.c -o common_pc.o
	cp -r common_pc.o ../meimei/pc_spider_s
	cp -r common_pc.o ../meimei/pc_socket_test

.PHONNY : clean
clean:
	rm $(objects)
