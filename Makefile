#  cc = "/usr/local/bin/mpicxx"
#  GCC = "/usr/local/bin/mpicxx"
cc = "/usr/bin/mpicxx"
GCC = "/usr/bin/mpicxx"

# GraphFile = r_graph.txt
# PartitionFile = r_graph_process.txt
# GraphFile = or_undir.tab.txt

GraphFile = fl_compact.tab 
PartitionFile = fl_compact_part.4

# GraphFile = graphs/or_undir_new.txt
# PartitionFile = graphs/or_part_4.txt

rounds = 5
partitions = 4
#---------------------------------------------#
# Arguments for part1

# input_graph = graphs/or_undir.tab.txt
# output_graph = graphs/or_out.out
# partition_graph = graphs/or_part_4.txt
# rounds_part1 = 5
# partitions_part1 = 4

# #should we write the entire graph to a new file? 1 = yes
# new_file_flag = 1
# new_file_name = graphs/or_undir_new.txt

all: prog prog2

prog: prog1.o
	$(cc) -g prog1.o -o prog

prog1.o: prog1.cpp
	$(cc) -g -c prog1.cpp

prog2: prog2.cpp
	$(GCC) prog2.cpp -o prog2

run: prog2
	mpirun -np $(partitions) ./prog2 $(GraphFile) $(PartitionFile) $(rounds) $(partitions)

part1:
	./prog $(input_graph) $(output_graph) $(partition_graph) $(rounds_part1) $(partitions_part1) $(new_file_flag) $(new_file_name)

clean:
	rm *.o *.out prog prog2 graphs/*.out

# target: dependencies
# 	action
# fl_undir.tab.txt fl_out.txt 1