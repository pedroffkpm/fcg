#include "keyboard.h"
/**
Key press event handler
*/

bool rightPressed = false;
bool leftPressed = false;
bool upPressed = false;
bool downPressed = false;
bool turningRight = false;
bool turningLeft = false;
float turning = 0;

void onKeyDown(unsigned char key, int x, int y) {
	//printf("%d \n", key);
	switch (key) {
		case 'W':
        case 'w':
			upPressed = true;
			break;
        case 'S':
        case 's':
			downPressed = true;
			break;
        case 'A':
        case 'a':
			leftPressed = true;
			break;
        case 'D':
        case 'd':
			rightPressed = true;
			break;
		default:
			break;
	}
}

/**
Key release event handler
*/
void onKeyUp(unsigned char key, int x, int y) {
	switch (key) {
        case 'W':
        case 'w':
			upPressed = false;
			break;
        case 'S':
        case 's':
			downPressed = false;
			break;
        case 'A':
        case 'a':
			leftPressed = false;
			break;
        case 'D':
        case 'd':
			rightPressed = false;
			break;
		default:
			break;
	}
}

