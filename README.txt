How to build and run:
to build do the following:
open a terminal and cd into the source directory
run the following command:
g++ *.cpp

To run the program do the following:
an example of how to run sample5.txt:
./a.out sample5.txt 150

Interupt timer interval:
supports numbers on interval [1, 2147483647]

Listing of my Files:
sample5.txt - Save the Snail Game. Please note the comments on the top of the
              source file if the game does not animate in your own CPU sim
              implementation.

Source files:
Main.cpp - contains the main function. Does some parameter checking and
           parses the program source into a string vector to pass to ComputerSim.
ComputerSim.cpp - sets up the pipes and forks. It creates the CPU or Memory objects
                  depending on if it is a parent or child. Also passes the program
                  string vector to the Memory.
Memory.cpp - initializes the memory from the string vector. Signals the CPU when
             it is ready and then listens to commands from the cpu to either read
             or write.
Cpu.cpp - fetches instructions from the memory and executes them.

Header files:
ComputerSim.h
Memory.h
Cpu.h
Main.cpp
