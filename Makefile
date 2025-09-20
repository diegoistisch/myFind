all: myfind

myfind: myfind.c
	gcc -std=c11 -Wall -Wextra -o myfind myfind.c

clean:
	rm -f myfind