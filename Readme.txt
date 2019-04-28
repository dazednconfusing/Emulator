Sebastian Peralta peralas@seas.upenn.edu
Anish Jain jainani@seas.upenn.edu

-Files:
beep.aiff
emul8.c
gamul.c
gamul.h
Makefile
PONG
Readme.txt

-Overview of Work:
We were able to successfully implement the Gamul8 ISA in c which can be seen by running Gamul8 assembly instructions.

-Division of labor:
The overall architechture was worked on together (coded by Sebastian) and the opcode implementations were coded by the both of us.

-Compalition:
'make'

-Run:
'./emulator FILENAME'

-Code description:
gamul8 contains all state values, opcode implementations, errors handling as well as the fetch and execution functions. emul8 calls the fetch and execution functions in render() as well as the state values relating to keyboard input.

-Note:
Run each game in a large enough window (maximized if possible) for optimal performance. If the window is too small, a bug may cause the program to quit unexpectedly upon keyboard inputs of '4' and 'r'. We believe that this bug may be machine-specific as we have isolated the occurence to a single line in the display function in gamul8. This line simply updates specific values of the gamer struct but the value of our emulated program counter inexplicably jumps past all assembly instructions. This bug occurs only when '4' or 'r' is pressed and only on small windows.

