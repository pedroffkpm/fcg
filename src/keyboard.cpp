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
bool spacePressed = false;
bool fPressed = false;
float turning = 0;

bool changecam = false;
int cam = 0;

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
        case 'V':
        case 'v':
            if(!changecam){
            changecam = true;
            cam++;
            if(cam > 2)
                cam = 0;
            }
            break;
        case 'F':
        case 'f':
            if(!fPressed)
                fPressed = true;
            break;
        case 32:
            if(!spacePressed)
                spacePressed = true;
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
		case 'V':
        case 'v':
            changecam = false;
            break;
		default:
			break;
	}
}

