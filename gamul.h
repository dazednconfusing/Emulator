#ifndef GAMUL_H
#define GAMUL_H

#define SCREEN_WIDTH	64
#define SCREEN_HEIGHT	32
#define FONTSET_SIZE	80

typedef struct gamul8 {
	unsigned char display[SCREEN_WIDTH][SCREEN_HEIGHT];
} gamul8;

extern void gamul8_init(char* file_name, unsigned char* keypress);

extern unsigned short fetch();

extern void execute(unsigned short opcode, gamul8* gamer, unsigned char* keys,
	unsigned char* keypress);


#endif