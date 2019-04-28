/* ESE 350, Spring 2018
 * ------------------
 * Sebastian Peralta
 * Anish Jain
 */

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "gamul.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 320

//architecture we are emulating, refer gamul.h
gamul8 gamer;


//boolean state of keys pressed
unsigned char keys[16];
unsigned char keypress = 0;


// method to draw a single pixel on the screen
void draw_square(float x_coord, float y_coord);

// update and render logic, called by glutDisplayFunc
void render();

// idling function for animation, called by glutIdleFunc
void idle();

// initializes GL 2D mode and other settings
void initGL();

// function to handle user keyboard input (when pressed) 
void your_key_press_handler(unsigned char key, int x, int y);

// function to handle key release
void your_key_release_handler(unsigned char key, int x, int y);


/*	FUNCTION: main
 *  --------------
 *	Main emulation loop. Loads ROM, executes it, and draws to screen.
 *  You may also want to call here the initialization function you have written
 *  that initializes memory and registers.
 *	PARAMETERS:
 *				argv: number of command line arguments
 *				argc[]: array of command line arguments
 *	RETURNS:	usually 0
 */
int main(int argc, char *argv[])
{
	// seed random variable for use in emulation
	srand(time(NULL));

	// initialize GLUT
	glutInit(&argc, argv);

	// initialize display and window
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("ESE 519 - Gamul8r");

	// initialize orthographic 2D view, among other things
	initGL();
	 
	// handle key presses and releases
	glutKeyboardFunc(your_key_press_handler);
	glutKeyboardUpFunc(your_key_release_handler);

	// GLUT draw function
	glutDisplayFunc(render);

	// GLUT idle function, causes screen to redraw
	glutIdleFunc(idle);

	if (argc == 2) {

		gamul8_init(argv[1], &keypress);
		
		// main loop, all events processed here
		glutMainLoop();

		return 0;
	} else {
		printf("%s\n", "Please provide a file_name in your arguments." );
	}
}

/*	FUNCTION: draw_square
 *  ----------------------
 *	Draws a square. Represents a single pixel
 *  (Up-scaled to a 640 x 320 display for better visibility)
 *	PARAMETERS: 
 *	x_coord: x coordinate of the square
 *	y_coord: y coordinate of the square
 *	RETURNS: none
 */
void draw_square(float x_coord, float y_coord)
{
	// draws a white 10 x 10 square with the coordinates passed

	glBegin(GL_QUADS);  //GL_QUADS treats the following group of 4 vertices 
						//as an independent quadrilateral

		glColor3f(1.0f, 1.0f, 1.0f); 	//color in RGB
										//values between 0 & 1
										//E.g. Pure Red = (1.0f, 0.0f, 0.0f)
		glVertex2f(x_coord, y_coord);			//vertex 1
		glVertex2f(x_coord + 10, y_coord);		//vertex 2
		glVertex2f(x_coord + 10, y_coord + 10);	//vertex 3
		glVertex2f(x_coord, y_coord + 10);		//vertex 4

		//glVertex3f lets you pass X, Y and Z coordinates to draw a 3D quad
		//only if you're interested

	glEnd();
}

/*	FUNCTION: render
 *  ----------------
 *	GLUT render function to draw the display. Also emulates one
 *	cycle of emulation.
 *	PARAMETERS: none
 *	RETURNS: none
 */


void render()
{

	// clears screen
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	
	glLoadIdentity();

	//display_func(&gamer, 10, registers, &iReg, memory);
	unsigned short opcode = fetch();
	execute(opcode, &gamer, keys, &keypress);

	// draw a pixel for each display bit
	int i, j;
	for (i = 0; i < SCREEN_WIDTH; i++) {
		for (j = 0; j < SCREEN_HEIGHT; j++) {
			if (gamer.display[i][j] == 1) {
				draw_square((float)(i * 10), (float)(j * 10));
			}
		}
	}

	// swap buffers, allows for smooth animation
	glutSwapBuffers();
}

/*	FUNCTION: idle
 *  -------------- 
 *	GLUT idle function. Instructs GLUT window to redraw itself
 *	PARAMETERS: none
 *	RETURNS: none
 */
void idle()
{
	// gives the call to redraw the screen
	glutPostRedisplay();
}

/*	FUNCTION: initGL
 *  ----------------
 *	Initializes GLUT settings.
 *	PARAMETERS: none
 *	RETURN VALUE: none
 */
void initGL()
{
	// sets up GLUT window for 2D drawing
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, WINDOW_WIDTH, WINDOW_HEIGHT, 0.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// clears screen color (RGBA)
	glClearColor(0.f, 0.f, 0.f, 1.f);
}

/*	FUNCTION: your_key_press_handler
 *  --------------------------------
 *	Handles all keypresses and passes them to the emulator.
 *  This is also where you will be mapping the original GAMUL8
 *  keyboard layout to the layout of your choice
 *  Something like this:
 *  |1|2|3|C|		=>		|1|2|3|4|
 *	|4|5|6|D|		=>		|Q|W|E|R|
 *	|7|8|9|E|		=>		|A|S|D|F|
 *	|A|0|B|F|		=>		|Z|X|C|V|
 *	PARAMETERS: 
 *	key: the key that is pressed.
 *	x: syntax required by GLUT
 *	y: syntax required by GLUT
 *  (x & y are callback parameters that indicate the mouse location
 *  on the window. We are not using the mouse, so they won't be used, 
 *  but still pass them as glutKeyboardFunc needs it.) 
 *	RETURNS: none
 *  NOTE: If you intend to handle this in a different manner, you need not
 *  write this function.
 */
void your_key_press_handler(unsigned char key, int x, int y) {
	switch (key) {
		case '1':
			keys[0x1] = 1;
			break;
		case '2':
			keys[0x2] = 1;
			break;
		case '3':
			keys[0x3] = 1;
			break;
		case '4':
			keys[0xC] = 1;
			break;
		case 'q':
			keys[0x4] = 1;
			break;
		case 'w':
			keys[0x5] = 1;
			break;
		case 'e':
			keys[0x6] = 1;
			break;
		case 'r':
			keys[0xD] = 1;
			break;
		case 'a':
			keys[0x7] = 1;
			break;
		case 's':
			keys[0x8] = 1;
			break;
		case 'd':
			keys[0x9] = 1;
			break;
		case 'f':
			keys[0xE] = 1;
			break;
		case 'z':
			keys[0xA] = 1;
			break;
		case 'x':
			keys[0x0] = 1;
			break;
		case 'c':
			keys[0xB] = 1;
			break;
		case 'v':
			keys[0xF] = 1;
			break;
		default:
			//keypress = 0;
		;
	} 
	keypress = 1;
}

/*	FUNCTION: your_key_release_handler
 *  --------------------------------
 *	Tells emulator if any key is released. You can maybe give
 *  a default value if the key is released.
 *	PARAMETERS: 
 *	key: the key that is pressed.
 *	x: syntax required by GLUT
 *	y: syntax required by GLUT
 *  (x & y are callback parameters that indicate the mouse location
 *  on the window. We are not using the mouse, so they won't be used, 
 *  but still pass them as glutKeyboardFunc needs it.) 
 *	RETURNS: none
 *  NOTE: If you intend to handle this in a different manner, you need not
 *  write this function.
 */
void your_key_release_handler(unsigned char key, int x, int y) {
	switch (key) {
		case '1':
			keys[0x1] = 0;
			break;
		case '2':
			keys[0x2] = 0;
			break;
		case '3':
			keys[0x3] = 0;
			break;
		case '4':
			keys[0xC] = 0;
			break;
		case 'q':
			keys[0x4] = 0;
			break;
		case 'w':
			keys[0x5] = 0;
			break;
		case 'e':
			keys[0x6] = 0;
			break;
		case 'r':
			keys[0xD] = 0;
			break;
		case 'a':
			keys[0x7] = 0;
			break;
		case 's':
			keys[0x8] = 0;
			break;
		case 'd':
			keys[0x9] = 0;
			break;
		case 'f':
			keys[0xE] = 0;
			break;
		case 'z':
			keys[0xA] = 0;
			break;
		case 'x':
			keys[0x0] = 0;
			break;
		case 'c':
			keys[0xB] = 0;
			break;
		case 'v':
			keys[0xF] = 0;
			break;
		default:
			;
		;
	} 
	 keypress = 0;
}
