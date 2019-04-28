#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gamul.h"

unsigned char memory[4096];

//V0 through VE used by game, VF used as flag
unsigned char registers[16];

//0x000 through 0xFFF
unsigned short i;

//0x000 through 0xFFF
unsigned short pc;

unsigned short stack[16];
unsigned char sp;

//when delayReg and soundReg are non-zero must automatically decrement at rate of 60 Hz
unsigned char delay_timer;
unsigned char sound_timer;



//exits program and prints invalid opcode acquired to terminal
void opcode_error(unsigned short opcode);
//prints values of pc, opcodes, and delay register
void debug(unsigned short pc, unsigned char delayReg, unsigned short opcode);

// font set for rendering
const unsigned char fontset[FONTSET_SIZE] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0,		// 0
	0x20, 0x60, 0x20, 0x20, 0x70,		// 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0,		// 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0,		// 3
	0x90, 0x90, 0xF0, 0x10, 0x10,		// 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0,		// 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0,		// 6
	0xF0, 0x10, 0x20, 0x40, 0x40,		// 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0,		// 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0,		// 9
	0xF0, 0x90, 0xF0, 0x90, 0x90,		// A
	0xE0, 0x90, 0xE0, 0x90, 0xE0,		// B
	0xF0, 0x80, 0x80, 0x80, 0xF0,		// C
	0xE0, 0x90, 0x90, 0x90, 0xE0,		// D
	0xF0, 0x80, 0xF0, 0x80, 0xF0,		// E
	0xF0, 0x80, 0xF0, 0x80, 0x80		// F
};


/*	FUNCTION: load_file
 *  -------------------
 *	Loads the given program/game
 *	PARAMETERS: 
 *  file_name: name of file to be loaded
 *  buffer: system memory which will hold program
 *	RETURNS: 0 if successful, -1 if file error
 */
int load_file(char *file_name, unsigned char *buffer)
{
	FILE *file;
	int file_size;

	// open file stream in binary read-only mode
	if (!(file = fopen(file_name, "rb"))) {
		printf("%s\n", "No such file");
		exit(0);
	}	//man 3 fopen
	
	fseek(file, 0, SEEK_END);	//man 3 fseek

	file_size = ftell(file);	//man 3 ftell

	if (file_size > (4096 - 512)) {
		printf("%s\n", "File is too large");
		exit(0);
	}

	rewind(file);				//man 3 rewind
	
	fread(buffer, 1, file_size, file);	//man 3 fread
	return 0;

}


/*	FUNCTION: display_func
 *  ----------------------
 *	Sample function that displays a pixel on the screen
 *	PARAMETERS: 
 *  gamer: architecture to be emulated, defined in gamul.h
 *	RETURNS: none
 */

//Initialize registers and memory
//instructions begin at 0x200
void gamul8_init(char* file_name, unsigned char* keypress) {

	

	//initialize registers
	load_file(file_name, &memory[0x200]);
	
		pc = 0x200;


		//initialize fontset
		for (int j = 0; j < FONTSET_SIZE; j++) {
			memory[j] = fontset[j];
		}

		keypress = 0;
}


//fetches opcode from memory and increments the program counter accordingly
unsigned short fetch() {
	unsigned char insnHigh = memory[(pc)++];
	unsigned char insnLow = memory[(pc)++];
	unsigned short insn = (insnHigh << 8) | insnLow;
	return insn;
}

/* --------------------- Implemented Opcodes -----------------------------------------------------------------------------------------------*/

void clear_screen(gamul8* gamer) {
	for (int i = 0; i < SCREEN_WIDTH; i++) {
		for (int j = 0; j < SCREEN_HEIGHT; j++) {
			gamer-> display[i][j] = 0;
		}			
	}
}

void return_from_sub() {
	pc = stack[(sp)--];
}

void jump_address(unsigned short opcode) {
	pc = (opcode & 0x0FFF);
}

void call_subrtn(unsigned short opcode) {
	stack[++(sp)] = pc;
	pc = (opcode & 0x0FFF);
}

void skip_ifequal_const(unsigned short opcode) {
	if (registers[(opcode >> 8) & 0xF] == (opcode & 0xFF)) {
		pc = (pc) + 2;
	}
}

void skip_ifnotequal_const(unsigned short opcode) {
	if (registers[(opcode >> 8) & 0xF] != (opcode & 0xFF)) {
		pc = (pc) + 2;
	}
}

void skip_ifequal(unsigned short opcode) {
	if (registers[(opcode >> 8) & 0xF] == registers[(opcode >> 4) & 0xF]) {
		pc = (pc) + 2;
	}
}

void set_const(unsigned short opcode) {
	registers[(opcode >> 8) & 0xF] = (opcode & 0xFF);
}

void add_const(unsigned short opcode) {
	registers[(opcode >> 8) & 0xF] += (opcode & 0xFF);
}

void assign(unsigned short opcode) {
	registers[(opcode >> 8) & 0xF] = registers[(opcode >> 4) & 0xF];
}

void bit_or(unsigned short opcode) {
	registers[(opcode >> 8) & 0xF] |= registers[(opcode >> 4) & 0xF];
}

void bit_and(unsigned short opcode) {
	registers[(opcode >> 8) & 0xF] &= registers[(opcode >> 4) & 0xF];
}

void bit_xor(unsigned short opcode) {
	registers[(opcode >> 8) & 0xF] ^= registers[(opcode >> 4) & 0xF];
}

void add(unsigned short opcode) {
	unsigned short sum = registers[(opcode >> 8) & 0xF] + registers[(opcode >> 4) & 0xF];
	if (sum > 0xFF) {
		registers[0xF] = 1;
	} else {
		registers[0xF] = 0;
	}
	registers[(opcode >> 8) & 0xF] += registers[(opcode >> 4) & 0xF];
}

void subtract_from(unsigned short opcode) {
	signed short diff = registers[(opcode >> 8) & 0xF] - registers[(opcode >> 4) & 0xF];
	if (diff < 0) {
		registers[0xF] = 0;
	} else {
		registers[0xF] = 1;
	}
	registers[(opcode >> 8) & 0xF] -= registers[(opcode >> 4) & 0xF];
}

void shift_right(unsigned short opcode) {
	registers[0xF] = registers[(opcode >> 8 & 0xF)] & 0x1;
	registers[(opcode >> 8) & 0xF] >>= 1;
}

void subtract(unsigned short opcode) {
	signed short diff = registers[(opcode >> 4) & 0xF] - registers[(opcode >> 8) & 0xF];
	if (diff < 0) {
		registers[0xF] = 0;
	} else {
		registers[0xF] = 1;
	}
	registers[(opcode >> 8) & 0xF] = registers[(opcode >> 4) & 0xF] - registers[(opcode >> 8) & 0xF];
}

void shift_left(unsigned short opcode) {
	registers[0xF] = registers[(opcode >> 8) & 0xF] >> 7;
	registers[(opcode >> 8) & 0xF] <<= 1;
}

void skip_ifnotequal(unsigned short opcode) {
	if (registers[(opcode >> 8) & 0xF] != registers[(opcode >> 4) & 0xF]) {
		pc = (pc) + 2;
	}
}

void set_i(unsigned short opcode) {
	i = (opcode & 0xFFF);
}

void jump_v0(unsigned short opcode) {
	pc = (opcode & 0xFFF) + registers[0];
}

void set_rand(unsigned short opcode) {
	registers[(opcode >> 8) & 0xF] = (opcode & 0xFF) & (rand() % 255);
}

void display_func(gamul8 *gamer, unsigned short opcode) {
	unsigned char x = registers[(opcode >>8) & 0xF];
	unsigned char y = registers[(opcode >> 4) & 0xF];
	unsigned char n = opcode & 0xF;
	unsigned char hasFlipped = 0;
	for (int j = y; j < (y + n); j++) {
		for (int k = x; k < (x + 8); k++) {
			unsigned char new_pix = (memory[(i) + (j-y)] >> (7-(k-x))) & 0x1;
			unsigned char prev_pix;
			if (k >= SCREEN_WIDTH) {
				prev_pix = gamer->display[k - SCREEN_WIDTH][j];
				gamer->display[k - SCREEN_WIDTH][j] ^= new_pix;
			} else {
				prev_pix = gamer->display[k][j];
				//HERE IS THE BUG
				//printf("%s", "pc upon entering disp: ");
				//printf("%x\n", pc);
				gamer->display[k][j] = new_pix ^ (gamer->display[k][j]);
				//printf("%s", "pc upon leaving disp: ");
				//printf("%x\n", pc);

			}
			if (prev_pix != (prev_pix ^ new_pix)) {
				hasFlipped = 1;
			}
		}
	}
	registers[0xF] = hasFlipped;
}

void skip_ifkeypressed(unsigned short opcode, unsigned char* keys) {
	if (keys[registers[(opcode >> 8) & 0xF]]) {
		pc = (pc) + 2;
	} 
}

void skip_ifkeyunpressed(unsigned short opcode, unsigned char* keys) {
	if (!(keys[registers[(opcode >> 8) & 0xF]])) {
		pc = (pc) + 2;
	} 
}

void set_fromdelay(unsigned short opcode) {
	registers[(opcode >> 8) & 0xF] = delay_timer;

}

void wait_forkeypress(unsigned short opcode,
	 unsigned char* keys, unsigned char* keypress) {
	while (!keypress);
	for (int j = 0; j < 16; j++) {
		if (keys[j]) {
			registers[(opcode >> 8) & 0xF] = keys[j];
		}
	} 
}

void set_delay(unsigned short opcode) {
	delay_timer = registers[(opcode << 8) & 0xF];
}

void set_sound(unsigned short opcode) {
	sound_timer = registers[(opcode << 8) & 0xF];
}

void add_toi(unsigned short opcode) {
	i += registers[(opcode << 8) & 0xF];
}

//TODO implement
void set_ifromsprite(unsigned short opcode) {
	i = 5 * registers[(opcode >> 8) & 0xF];

}

void store_binaryof(unsigned short opcode) {
	unsigned char val = registers[(opcode >> 8) & 0xF];
	unsigned char dig1 = 0;
	unsigned char dig2 = 0;
	unsigned char dig3 = 0;
	if (val >= 100) {
		dig1 = 1;
		val -= 100;
	}
	for (int j = 9; j > 0; j--) {
		if (val >= j * 10) {
			dig2 = j;
			val -= j* 10;
			break;
		}
	}
	for (int j = 9; j > 0; j--) {
		if (val == j) {
			dig3 = j;
			break;
		}
	}
	memory[i] = dig1;
	memory[(i) + 1] = dig2;
	memory[(i) + 2] = dig3;
}

void store_registers(unsigned short opcode) {
	for (unsigned char j = 0; j <= ((opcode >> 8) & 0xF); j++) {
		memory[(i) + j] = registers[j];
	}
}

void fill_registers(unsigned short opcode) {
	for (unsigned char j = 0; j <= ((opcode >> 8) & 0xF); j++) {
		registers[j] = memory[(i) + j];
	}
}

void handle_timers() {
	if (sound_timer > 0) {
		system("paplay beep.aiff &> /dev/null &");
		(sound_timer)--;
	}
	if (delay_timer > 0) {
		(delay_timer)--;
	}
}

//executes each instruction and updates relative paramaters
 void execute(unsigned short opcode, gamul8 *gamer, unsigned char* keys,
 		unsigned char* keypress) {
 	switch (opcode >> 12) {
		case 0x0:
			switch (opcode & 0xF) {
				case 0x0:
					//clear screen
					clear_screen(gamer);
					break;
					
				case 0xE:
					//Returns from a subroutine
					return_from_sub();
					break;
				default: 
					opcode_error(opcode);
					break;
			}
			break;
		case 0x1:
			//jumps to address NNN
			jump_address(opcode);
			break;
		case 0x2:
			//Calls subroutine at NNN
			call_subrtn(opcode);
			break;
		case 0x3:
			//Skips the next instruction if VX equals NN
			skip_ifequal_const(opcode);
			break;

		case 0x4:
			//Skips the next instruction if VX doesnt equal NN
			skip_ifnotequal_const(opcode);
			break;
		case 0x5:
			//Skips the next instruction if VX equals VY
			if (!(opcode & 0xF)) {
				skip_ifequal(opcode);
			} else {
				opcode_error(opcode);
			}
			break;
		case 0x6:
			//set VX to NN
			set_const(opcode);
			break;
		case 0x7:
			//add NN to VX
			add_const(opcode);
			break;
		case 0x8:
			switch (opcode & 0xF) {
				case 0x0:
					//assign VY to VX
					assign(opcode);
					break;
				case 0x1:
					//sets VX to VX or VY
					bit_or(opcode);
					break;
				case 0x2:
					//sets VX to VX and VY
					bit_and(opcode);
					break;
				case 0x3:
					//sets VX to VX xor VY
					bit_xor(opcode);
					break;
				case 0x4:
					//adds VY to VX and sets flag if carry
					add(opcode);
					break;
				case 0x5:
					//subtracts VX from VY and ssets flag if there is no borrow
					subtract_from(opcode);
					break;
				case 0x6:
					//shifts VX right by 1 and sets VF to LSB before shift
					shift_right(opcode);
					break;
				case 0x7:
					//sets VX to VY - VX and sets flag if there is no borrow
					subtract(opcode);
					break;
				case 0xE:
					//shifts VX left by 1 and sets VF to MSB before shift
					shift_left(opcode);
					break;
				default:
					opcode_error(opcode);
					break;
			}
			break;
		case 0x9:
			 //skip if VX doesnt equal VY
			if (!(opcode & 0xF)) {
				skip_ifnotequal(opcode);
			} else {
				opcode_error(opcode);
			}
			break;
		case 0xA:
			//sets i to NNN
			set_i(opcode);
			break;
		case 0xB:
			//jumps to address NNN + v0
			jump_v0(opcode);
			break;
		case 0xC:
			//sets VX to rand & NN
			set_rand(opcode);
			break;
		case 0xD:
			//draws sprite 
			display_func(gamer, opcode);
			break;
		case 0xE:
			switch (opcode & 0xFF) {
				case 0x9E:
					//skip if key is set
					skip_ifkeypressed(opcode, keys);
					break;
				case 0xA1:
					//skip if key is not set
					skip_ifkeyunpressed(opcode, keys);
					break;
				default:
					opcode_error(opcode);
					break;
			}
			break;
		case 0xF:
			switch (opcode & 0xFF) {
				case 0x07:
					//sets VX to value of delay timer
					set_fromdelay(opcode);
					break;
				case 0x0A:
					//wait for key press
					wait_forkeypress(opcode, keys, keypress);
					break;
				case 0x15:
					//set delay timer to VX
					set_delay(opcode);
					break;
				case 0x18:
					//set sound timer to VX
					set_sound(opcode);
					break;
				case 0x1E:
					//adds VX to I
					add_toi(opcode);
					break;
				case 0x29:
					//sets i from sprite 
					set_ifromsprite(opcode);
					break;
				case 0x33:
					//stores decimal digits of VX in memory starting at i
					store_binaryof(opcode);
					break;
				case 0x55:
					//stores V0 through VX in memory starting at i
					store_registers(opcode);
					break;
				case 0x65:
					//fills V0 through VX from memory starting at i
					fill_registers(opcode);
					break;
				default: 
					opcode_error(opcode);
					break; 
			} 
			break;
			default: 
				opcode_error(opcode);

	}
	handle_timers();
	//debug(pc, delay_timer, opcode);

}

//errors
void opcode_error(unsigned short opcode) {
	printf("%s", "Invalid opcode: ");
	printf("%x\n", opcode);
	exit(0);
}

void debug(unsigned short pc, unsigned char delayReg, unsigned short opcode) {
	printf("%s", "opcode: ");
	printf("%x", opcode);
	printf("%s", ", pc: ");
	printf("%x", pc);
	printf("%s", ", delay_timer: ");
	printf("%x\n", delayReg);
}