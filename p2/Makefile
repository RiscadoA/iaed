CC=gcc
CFLAGS=-Wall -Wextra -Werror -ansi -pedantic -g
all:: proj2
	$(MAKE) $(MFLAGS) -C tests
proj2: proj2.c file.c avl.c table.c list.c
clean::
	rm -f proj2 a.out *.o core tests/*.diff