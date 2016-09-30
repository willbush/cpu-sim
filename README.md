# cpu-sim

### How to build and run
Because I'm too lazy to make a build file, do the following to build:
open a terminal and cd into the cpu-sim directory
run the following command:

```bash
find . -name '*.cpp' ! -name 'Test.cpp' | xargs g++ -o cpu-sim
```
The command will find and compile all the cpp files except the poorly hacked together unit test file, which won't compile unless you have CUTE test framework library available. 

An example of how to run the program with the snail game:
```bash
./cpu-sim snailGame.txt 150
```

Interrupt timer interval:
supports numbers on interval [1, 2147483647]

### Save the Snail Game
The snail races to a stamper, if the stamper is shut
when the snail reaches it, then the snail is going to get
squashed. The rate at which the stamper opens and slams
is controlled by how often the interrupt handler gets called.
the interrupt handler is called based on the timer parameter
that you pass to the program. So in order to win you need to
find a timer interval that allows the snail to reach the
stamper with it open.

IMPORTANT NOTE: Game animation depends on how printing a char
is implemented in the CPU simulator. The buffer needs to be
disabled or flushed each time after printing the character.

#### Listing of my Files

snailGame.txt - Save the Snail Game. Please note the comments on the top of the
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
