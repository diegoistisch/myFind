.DEFAULT_GOAL := myfind.c

all: myfind

myfind.c: 
	gcc -o ./myfind.out ./myfind.c

run: myfind.c
	./myfind.out .
clean:
	rm -f myfind.out