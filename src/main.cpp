#include <iostream>
#include <GL/gl.h>
#include <glut.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cmath>

#include "glm.h"
#include "keyboard.h"
#include "window.h"
//openal (sound lib)
#include <al/alut.h>
//bitmap class to load bitmaps for textures
#include "bitmap.h"
#pragma comment(lib, "OpenAL32.lib")
#pragma comment(lib, "alut.lib")
#define PI 3.14159265

// sound stuff
#define NUM_BUFFERS 1
#define NUM_SOURCES 1
#define NUM_ENVIRONMENTS 1

#define SMOOTH 0
#define SMOOTH_MATERIAL 1
#define SMOOTH_MATERIAL_TEXTURE 2

void mainInit();
void initSound();
void initTexture();
void initModel();
void initLight();
void enableFog();
void createGLUI();
void mainRender();
void mainCreateMenu();
void onGLUIEvent(int id);
void onWindowReshape(int x, int y);
void mainIdle();
int main(int argc, char **argv);
void setWindow();
void setViewport(GLint left, GLint right, GLint bottom, GLint top);
void updateState();
void renderFloor();
void updateCam();

int mainWindowId = 0;

double xOffset = -1.9;
double yOffset = -1.3;
int mouseLastX = 0;
int mouseLastY = 0;

float roty = 0.0f;
float rotx = 90.0f;

bool spacePressed = false;

float speedX = 0.0f;
float speedY = 0.0f;
float speedZ = 0.0f;



int angulo = 0;
/*
variavel auxiliar pra dar variação na altura do ponto de vista ao andar.
*/
float headPosAux = 0.0f;

float maxSpeed = 0.25f;

float planeSize = 8.0f;

// more sound stuff (position, speed and orientation of the listener)
ALfloat listenerPos[]={0.0,0.0,4.0};
ALfloat listenerVel[]={0.0,0.0,0.0};
ALfloat listenerOri[]={0.0,0.0,1.0,
						0.0,1.0,0.0};

// now the position and speed of the sound source
ALfloat source0Pos[]={ -2.0, 0.0, 0.0};

ALfloat source0Vel[]={ 0.0, 0.0, 0.0};
// buffers for openal stuff
ALuint  buffer[NUM_BUFFERS];
ALuint  source[NUM_SOURCES];
ALuint  environment[NUM_ENVIRONMENTS];
ALsizei size,freq;
ALenum  format;
ALvoid  *data;



// parte de código extraído de "texture.c" por Michael Sweet (OpenGL SuperBible)
// texture buffers and stuff
int i;                       /* Looping var */
BITMAPINFO	*info;           /* Bitmap information */
GLubyte	    *bits;           /* Bitmap RGB pixels */
GLubyte     *ptr;            /* Pointer into bit buffer */
GLubyte	    *rgba;           /* RGBA pixel buffer */
GLubyte	    *rgbaptr;        /* Pointer into RGBA buffer */
GLubyte     temp;            /* Swapping variable */
GLenum      type;            /* Texture type */
GLuint      texture;         /* Texture object */


/**
bool crouched = false;
bool running = false;
bool jumping = false;*/
float jumpSpeed = 0.06;
float gravity = 0.004;
float heightLimit = 0.2;


float backgrundColor[4] = {0.0f,0.0f,0.0f,1.0f};

int modelos_carregados = 0;
struct object
{
    GLMmodel *model;
    int x;
    int z;
} models[500];

// Aux function to load the object using GLM and apply some functions
bool C3DObject_Load_New(const char *pszFilename, GLMmodel **model)
{
    char aszFilename[256];
    strcpy(aszFilename, pszFilename);

    if (*model) {

    free(*model);
    *model = NULL;
    }

    *model = glmReadOBJ(aszFilename);
    if (!(*model))
    return false;

    glmUnitize(*model);
    //glmScale(model,sFactor); // USED TO SCALE THE OBJECT
    glmFacetNormals(*model);
    glmVertexNormals(*model, 90.0);

    return true;
}



/**
Atualiza a posição e orientação da camera
*/
void updateCam() {

	gluLookAt(posX,posY,posZ,
		posX + sin(roty*PI/180),posY + cos(rotx*PI/180),posZ -cos(roty*PI/180),
		0.0,1.0,0.0);

    GLfloat light_position0[] = {posX, posY, posZ, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position0);

	GLfloat direcao[] = {sin(roty*PI/180), cos(rotx*PI/180), -cos(roty*PI/180)};
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, direcao);

	angulo+=5;
    GLfloat light_position1[] = {0.6*cos(angulo*M_PI/180.0), 0.5, 0.6*sin(angulo*M_PI/180.0)};
    glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
}

void initLight() {
    glEnable(GL_LIGHTING );
	glEnable( GL_LIGHT0 );
	glEnable( GL_LIGHT1 );

	GLfloat light_ambient[] = { backgrundColor[0], backgrundColor[1], backgrundColor[2], backgrundColor[3] };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_position0[] = {0.0, 0.0, 0.0, 1.0 };
    GLfloat light_position1[] = {0.0, 0.0, 0.0, 1.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 35.0);

	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

}



/**
Initialize
*/
void mainInit() {
	glClearColor(1.0,1.0,1.0,0.0);
	glColor3f(0.0f,0.0f,0.0f);
	setWindow();
	setViewport(0, windowWidth, 0, windowHeight);

	// habilita o z-buffer
	glEnable(GL_DEPTH_TEST);

    initTexture();

	initModel();

	initLight();

	enableFog();

	printf("w - andar \n");
	printf("s - ir pra tras \n");
	printf("mouse - direcao \n");
	printf("r - correr \n");
	printf("c - abaixar \n");
	printf("espaco - pular \n");

}

void initModel() {
	printf("Loading models.. \n");
	bits = LoadDIBitmap("map.bmp", &info);
	if (bits == (GLubyte *)0) {
		printf ("Error loading models!\n\n");
		return;
	}

	int xpos = 0;
    int zpos = (int)info->bmiHeader.biHeight - 1;
    bool modelLoaded = false;
	i = info->bmiHeader.biWidth * info->bmiHeader.biHeight;

    for(ptr = bits; i > 0; i--, ptr += 3)
    {
        int color = (ptr[2] << 16) + (ptr[1] << 8) + ptr[0];
        switch(color)
        {
        case 0x000000:
            break;
        case 0xff0000:
            modelLoaded = C3DObject_Load_New("../../models/OogieBoogie.obj", &models[modelos_carregados].model);
            break;
        case 0x00ff00:
            modelLoaded = C3DObject_Load_New("../../models/f-16.obj", &models[modelos_carregados].model);
            break;
        case 0x0000ff:
            modelLoaded = C3DObject_Load_New("../../models/flowers.obj", &models[modelos_carregados].model);
            break;
        case 0xffff00:
            modelLoaded = C3DObject_Load_New("../../models/flowers.obj", &models[modelos_carregados].model);
        default:
            printf("Unidentified color.");
        }

	if (modelLoaded){
		models[modelos_carregados].x = xpos;
		models[modelos_carregados].z = zpos;
		modelos_carregados++;
		modelLoaded = false;
		}
	xpos++;
	if(xpos ==(int)info->bmiHeader.biWidth){
	xpos = 0;
	zpos--;
	}
    }
	printf("Models ok. \n \n \n");
}


/**
Initialize the texture using the library bitmap
*/
void initTexture(void)
{
    printf ("\nLoading texture..\n");
    // Load a texture object (256x256 true color)
    bits = LoadDIBitmap("../../res/stones.bmp", &info);
    if (bits == (GLubyte *)0) {
		printf ("Error loading texture!\n\n");
		return;
	}
    // Figure out the type of texture
    if (info->bmiHeader.biHeight == 1)
      type = GL_TEXTURE_1D;
    else
      type = GL_TEXTURE_2D;

    // Create and bind a texture object
    glGenTextures(1, &texture);
	glBindTexture(type, texture);

    // Create an RGBA image
    rgba = (GLubyte *)malloc(info->bmiHeader.biWidth * info->bmiHeader.biHeight * 4);

    i = info->bmiHeader.biWidth * info->bmiHeader.biHeight;
    for( rgbaptr = rgba, ptr = bits;  i > 0; i--, rgbaptr += 4, ptr += 3)
    {
            rgbaptr[0] = ptr[2];     // windows BMP = BGR
            rgbaptr[1] = ptr[1];
            rgbaptr[2] = ptr[0];
            rgbaptr[3] = (ptr[0] + ptr[1] + ptr[2]) / 3;
    }

	// Set texture parameters
	glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexImage2D(type, 0, 4, info->bmiHeader.biWidth, info->bmiHeader.biHeight,
                  0, GL_RGBA, GL_UNSIGNED_BYTE, rgba );


    printf("Textura %d\n", texture);
	printf("Textures ok.\n\n", texture);

}

void enableFog(void)
{
    float fog_colour[4] = {0.5,0.5,0.5,1.0};
    glEnable(GL_FOG);
    glFogf(GL_FOG_MODE,GL_EXP2);
    glFogfv(GL_FOG_COLOR,fog_colour);
    glFogf(GL_FOG_DENSITY,0.2);
    glHint(GL_FOG_HINT,GL_NICEST);
    glFogf(GL_FOG_START, 5);
    glFogf(GL_FOG_END,100);
}

void renderFloor() {
	// set things up to render the floor with the texture
	glShadeModel(GL_SMOOTH);
	glEnable(type);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glPushMatrix();

    glTranslatef(-(float)planeSize/2.0f, 0.0f, -(float)planeSize/2.0f);

	float textureScaleX = 10.0;
	float textureScaleY = 10.0;
    glColor4f(1.0f,1.0f,1.0f,1.0f);
    int xQuads = 10;
    int yQuads = 10;
    int zQuads = 10;
    for (int i = 0; i < xQuads; i++) {
        for (int j = 0; j < zQuads; j++) {
            glBegin(GL_QUADS);
                glTexCoord2f(1.0f, 0.0f);   // coords for the texture
                glNormal3f(0.0f,1.0f,0.0f);
                glVertex3f(i * (float)planeSize/xQuads, 0.0f, (j+1) * (float)planeSize/zQuads);

                glTexCoord2f(0.0f, 0.0f);  // coords for the texture
                glNormal3f(0.0f,1.0f,0.0f);
                glVertex3f((i+1) * (float)planeSize/xQuads, 0.0f, (j+1) * (float)planeSize/zQuads);

                glTexCoord2f(0.0f, 1.0f);  // coords for the texture
                glNormal3f(0.0f,1.0f,0.0f);
                glVertex3f((i+1) * (float)planeSize/xQuads, 0.0f, j * (float)planeSize/zQuads);

                glTexCoord2f(1.0f, 1.0f);  // coords for the texture
                glNormal3f(0.0f,1.0f,0.0f);
                glVertex3f(i * (float)planeSize/xQuads, 0.0f, j * (float)planeSize/zQuads);

            glEnd();
        }

    }

	glDisable(type);


	glPopMatrix();
}

void renderScene() {
	glClearColor(backgrundColor[0],backgrundColor[1],backgrundColor[2],backgrundColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // limpar o depth buffer

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	updateCam();

	int cont;
	for (cont=0; cont<modelos_carregados; cont++)
	{

		glPushMatrix();
        glTranslatef(GLfloat(models[cont].x - 3.5), 0.5, GLfloat(models[cont].z - 3.5));
        glmDraw(models[cont].model, GLM_SMOOTH | GLM_MATERIAL);
		glPopMatrix();
	}

    // binds the bmp file already loaded to the OpenGL parameters
    glBindTexture(type, texture);

	renderFloor();
}

void updateState() {

	if (upPressed || downPressed) {

			speedX = 0.05 * sin(roty*PI/180);
			speedZ = -0.05 * cos(roty*PI/180);

        if (upPressed) {
            posX += speedX;
            posZ += speedZ;
        } else {
            posX -= speedX;
            posZ -= speedZ;
        }

	}
}

/**
Render scene
*/
void mainRender() {
	updateState();
	renderScene();
	glFlush();
	glutPostRedisplay();
	Sleep(30);
}

/**
Handles events from the mouse right button menu
*/
void mainHandleMouseRightButtonMenuEvent(int option) {
	switch (option) {
		case 1 :
			exit(0);
			break;
		default:
			break;
	}
}

/**
Create mouse button menu
*/
void mainCreateMenu() {
	glutCreateMenu(mainHandleMouseRightButtonMenuEvent);
	glutAddMenuEntry("Quit", 1);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}


/**
Glut idle funtion
*/
void mainIdle() {
	/**
	Set the active window before send an glutPostRedisplay call
	so it wont be accidently sent to the glui window
	*/

	glutSetWindow(mainWindowId);
	glutPostRedisplay();
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(windowWidth,windowHeight);
	glutInitWindowPosition(windowXPos,windowYPos);

	/**
	Store main window id so that glui can send it redisplay events
	*/
	mainWindowId = glutCreateWindow("DigDug");

	glutDisplayFunc(mainRender);

	glutReshapeFunc(onWindowReshape);

	/**
	Register keyboard events handlers
	*/
	glutKeyboardFunc(onKeyDown);
	glutKeyboardUpFunc(onKeyUp);

	mainInit();

	glutMainLoop();

    return 0;
}
