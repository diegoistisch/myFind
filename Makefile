all: myfind

myfind: myfind.c
	gcc -o myfind myfind.c

clean:
	rm -f myfind