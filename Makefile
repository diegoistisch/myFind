all: myfind

myfind: myfind.c
	gcc -std=c11 -o myfind myfind.c

clean:
	rm -f myfind