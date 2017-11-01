all: test_avl

test_avl: avl.o test_avl.c
	gcc -std=c99 -g test_avl.c avl.o -o test_avl

avl.o: avl.c avl.h
	gcc -std=c99 -g -c avl.c

clean:
	rm -f avl.o test_avl
	rm -rf *.dSYM/
