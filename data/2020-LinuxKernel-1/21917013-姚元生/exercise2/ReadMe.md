This exercise implements a bash simulator. The simulator can simulate the behavior of bash.
It offers built-in personalized commands such as run, list, kill and quit.

Prequisitie:
To install this project, **you need to install lib readline first**:
sudo apt-get install libreadline-dev

Install:
* use make:
cd to the exercise2 directory, then enter:
make all
* install the object manually:
gcc -o hello.o hello.c (optional, The sole purpose of this program is to facilitate the demonstration of the main program.) 
gcc -o shell.o shell.c -lreadline

Unistall:
* use make:
make clean
* Or you can delete *.o files manually.