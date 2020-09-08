CIS630, Distributed Systems 
Programming Project 1

Please fill this form and send it together with your program.
Put all the source files, makefile and this text file into a folder,
use your lastname as the name of the folder (all lowercase letters),
then compress the folder into a tarball, gz or zip and submit it through the TurnIN 
page.
(https://systems.cs.uoregon.edu/apps/turnin.php)
Please include any special instructions for compiling and running your program on ix-dev.

Student Names: NITHIN KRISHNA GOWDA
Student IDs: 951708588

Answer the questions by selecting one of the given options, or provide the requested information. 
You may add additional information if you wish to.

Questions about your submitted code:
-----------------------------------
- Does your code properly compiles on ix-dev?  yes

- Does your code provide the specified output (on screen and output file) with the proper format?  yes

- Does your code utilse excessive amount of memory?  no

- Does your code gracefully terminate?   yes

- Does your code produce the correct results?  yes

- Do multiple processes sync correctly in each round?  yes

- Are credits for all nodes calculated correctly (up to the first 5 decimal digits) across the first 5 rounds?  yes

- Does your code correctly run over 2 and 4 processes and terminates?  yes

- What is the total execution time (in second) of your code (for reading flikr input files with 4 partitions, 
5 rounds of calculation, and writing the results to the output) on ix-dev.cs.uoregon.edu?       execution time: 12.288 to 14.355 seconds

Note:

1. "prog2" is the executable file generated after running the command "make". The files "fl_compact.tab", "fl_compact_part.2" and "fl_compact_part.4" should be in the same directory as the executable so that the make file need not be changed. 
2. Use the command "make run2" to run the program with 2 partitions.
3. Use the command "make run4" to run the program with 4 partitions.

or the program can be run using the following commands:
mpirun -np 4 ./prog2 /path-to-file/fl_compact.tab /path-to-file/fl_compact_part.4 5 4
mpirun -np 2 ./prog2 /path-to-file/fl_compact.tab /path-to-file/fl_compact_part.2 5 2
