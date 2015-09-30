all:
	gcc -Wall --std=gnu99 -pthread -I/usr/include/malloc/ -o wave wave.c