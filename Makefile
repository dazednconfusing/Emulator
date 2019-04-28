emulator: emul8.c gamul.c gamul.h
	gcc emul8.c gamul.c -o emulator -lGL -lGLU -lglut -std=c99 -Wall
