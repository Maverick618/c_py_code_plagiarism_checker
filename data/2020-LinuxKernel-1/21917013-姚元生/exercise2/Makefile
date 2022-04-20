all: hello shell

hello : hello.o 
	gcc -o hello.o hello.c

shell : shell.o 
	gcc -o shell.o shell.c -lreadline

clean:
	rm *.o
