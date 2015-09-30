all:
	gcc -Wall -pthread -I/usr/include/malloc/ -o wave wave.c