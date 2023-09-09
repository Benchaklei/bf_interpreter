CFLAGS=-std=c17 -Wall -Wextra
all:
	gcc bf.c -o bf $(CFLAGS)