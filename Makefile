all: prog strtok

strtok: strtok.cpp
	g++ -o strtok strtok.cpp 

prog: prog1.o
	g++ prog1.o -o prog

prog1.o: prog1.cpp
	g++ -c prog1.cpp




clean:
	rm *.o *.out prog strtok

# target: dependencies
# 	action