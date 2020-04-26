# cc = "/usr/local/bin/mpicxx"
# GCC = "/usr/local/bin/mpicxx"
cc = "/usr/bin/mpicxx"
GCC = "/usr/bin/mpicxx"
GraphFile = fl_compact.tab 
#GraphFile = r_graph.txt
#GraphFile = or_undir.tab.txt
PartitionFile = fl_compact_part.4
#PartitionFile = r_graph_process.txt
rounds = 5
partitions = 4


all: prog prog2

strtok: strtok.cpp
	${cc} -g -o strtok strtok.cpp 

prog: prog1.o
	$(cc) -g prog1.o -o prog

prog1.o: prog1.cpp
	$(cc) -g -c prog1.cpp

prog2: prog2.cpp
	$(GCC) prog2.cpp -o prog2

run: prog2
	mpirun -np $(partitions) ./prog2 $(GraphFile) $(PartitionFile) $(rounds) $(partitions)




clean:
	rm *.o *.out prog strtok prog2 out0.txt out1.txt out2.txt out3.txt

# target: dependencies
# 	action
# fl_undir.tab.txt fl_out.txt 1