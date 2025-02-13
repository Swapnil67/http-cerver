CFLAGS=-Wall -Wextra -Wconversion -Werror -pedantic --std=c17 -ggdb
LIBS=

nodec: node.c
	$(CC) $(CFLAGS) -o nodec node.c
