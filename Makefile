all:
	gcc -pthread -I/usr/include/malloc/ -o thread thread.c