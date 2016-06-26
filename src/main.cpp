#include <windows.h>
#include <iostream>


#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glut.h>

#include <math.h>
#include <cmath>


#include "glm.h"
#include "keyboard.h"

#include "window.h"
#include "maps.h"
#include "models.h"
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
void initTextures();

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
void initPlayer();

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

int i;                       /* Looping var */
BITMAPINFO	*info;           /* Bitmap information */
GLubyte	    *bits;           /* Bitmap RGB pixels */
GLubyte     *ptr;            /* Pointer into bit buffer */
GLubyte	    *rgba;           /* RGBA pixel buffer */
GLubyte	    *rgbaptr;        /* Pointer into RGBA buffer */
GLubyte     temp;            /* Swapping variable */

int angulo = 0;
/*
variavel auxiliar pra dar variação na altura do ponto de vista ao andar.
*/
float headPosAux = 0.0f;

float maxSpeed = 0.25f;

//float planeSize = 8.0f;

// parte de código extraído de "texture.c" por Michael Sweet (OpenGL SuperBible)
// texture buffers and stuff


/**
bool crouched = false;
bool running = false;
bool jumping = false;*/
float gravity = 0.004;


float backgrundColor[4] = {0.8f,0.9f,1.0f,1.0f};

int modelos_carregados = 0;
struct object
{
    GLMmodel *model;
    float x;
    float z;
    float y;
} models[500], player;

// Aux function to load the object using GLM and apply some functions
bool C3DObject_Load_New(const char *pszFilename, GLMmodel **model)
{
    char aszFilename[256];
    strcpy(aszFilename, pszFilename);

    if (*model)
    {

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
void updateCam()
{

    if(cam == 0)
    {
        gluLookAt(player.x, player.y, player.z,
                  player.x + sin(roty*PI/180), 0.2, player.z - cos(roty*PI/180),
                  0.0,1.0, 0.0);


        GLfloat light_position0[] = {player.x, 0.2, player.z, 0.0 };
        glLightfv(GL_LIGHT0, GL_POSITION, light_position0);

        GLfloat direcao[] = {sin(roty*PI/180), cos(rotx*PI/180), -cos(roty*PI/180)};
        //glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, direcao);
    }
    if(cam == 1)
    {
        gluLookAt(player.x, player.y + 0.5f, player.z - 0.5f,
                  player.x + sin(roty*PI/180), 0.4, (player.z -0.5f) - cos(roty*PI/180),
                  0.0,1.0, 0.0);
    }
    if (cam == 2)
    {
        gluLookAt(15.5, 40.0, 15.5,
                  15.5, 0.0, 15.5,
                  0.0,0.0, -1.0);

    }
}

void initLight()
{
    glEnable(GL_LIGHTING );
    glEnable( GL_LIGHT0 );

    GLfloat light_ambient[] = { backgrundColor[0], backgrundColor[1], backgrundColor[2], backgrundColor[3] };
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_position0[] = {0.0, 0.0, 0.0, 0.0 };
    GLfloat light_position1[] = {0.0, 0.0, 0.0, 1.0 };

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
    //glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 90.0);

}

void initModel()
{
    printf("Loading models.. \n");
    bits = LoadDIBitmap("map.bmp", &info);
    if (bits == (GLubyte *)0)
    {
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

        if (modelLoaded)
        {
            models[modelos_carregados].x = xpos;
            models[modelos_carregados].z = zpos;
            modelos_carregados++;
            modelLoaded = false;
        }
        xpos++;
        if(xpos ==(int)info->bmiHeader.biWidth)
        {
            xpos = 0;
            zpos--;
        }
    }
    printf("Models ok. \n \n \n");
}

void initPlayer()
{
    for (int x = 0; x < 32; x++)
        for (int z = 0; z < 32; z++)
            if (mapXZ[x][z] == 4)
            {
                player.x = x;
                player.z = z;
            }

    player.y = 0.4f;

    printf("Loading player.. \n");


    bool modelLoaded = false;

    modelLoaded = C3DObject_Load_New("../../models/LEGO_man.obj", &player.model);


    if (!modelLoaded)
        printf("Erro ao carregar modelo!\n");
    else printf("Player Model ok. \n \n \n");
}

/**
Initialize the texture using the library bitmap
*/
void initTextures(void)
{
    printf ("\nLoading grass texture..\n");
    // Load a texture object (256x256 true color)
    bits = LoadDIBitmap("../../res/grass.bmp", &info);
    if (bits == (GLubyte *)0)
    {
        printf ("Error loading texture!\n\n");
        return;
    }
    // Figure out the type of texture
    if (info->bmiHeader.biHeight == 1)
        type = GL_TEXTURE_1D;
    else
        type = GL_TEXTURE_2D;

    // Create and bind a texture object
    glGenTextures(1, &grass);
    glBindTexture(type, grass);

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


    printf ("\nLoading grass_cracked texture..\n");
    // Load a texture object (256x256 true color)
    bits = LoadDIBitmap("../../res/grass_cracked2.bmp", &info);
    if (bits == (GLubyte *)0)
    {
        printf ("Error loading texture!\n\n");
        return;
    }
    // Figure out the type of texture
    if (info->bmiHeader.biHeight == 1)
        type = GL_TEXTURE_1D;
    else
        type = GL_TEXTURE_2D;

    // Create and bind a texture object
    glGenTextures(1, &grass_cracked);
    glBindTexture(type, grass_cracked);

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


    printf ("\nLoading grass_hole texture..\n");
    // Load a texture object (256x256 true color)
    bits = LoadDIBitmap("../../res/grass_hole.bmp", &info);
    if (bits == (GLubyte *)0)
    {
        printf ("Error loading texture!\n\n");
        return;
    }
    // Figure out the type of texture
    if (info->bmiHeader.biHeight == 1)
        type = GL_TEXTURE_1D;
    else
        type = GL_TEXTURE_2D;

    // Create and bind a texture object
    glGenTextures(1, &grass_hole);
    glBindTexture(type, grass_hole);

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

    printf("Textures ok.\n\n");

}

/**
Initialize
*/
void mainInit()
{
    glClearColor(1.0,1.0,1.0,0.0);
    glColor3f(0.0f,0.0f,0.0f);
    setWindow();
    setViewport(0, windowWidth, 0, windowHeight);

    // habilita o z-buffer
    glEnable(GL_DEPTH_TEST);

    initTextures();

    //initModel();

    openMap();

    initPlayer();

    initLight();

    //enableFog();
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


void renderScene()
{
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

    glPushMatrix();
    glTranslatef(GLfloat(player.x), GLfloat(player.y), GLfloat(player.z));
    glmDraw(player.model, GLM_SMOOTH | GLM_MATERIAL);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(GLfloat(4), GLfloat(player.y), GLfloat(4));
    glmDraw(player.model, GLM_SMOOTH | GLM_MATERIAL);
    glPopMatrix();

    // binds the bmp file already loaded to the OpenGL parameters


    renderFloor();
}

void updateState()
{
    int x = round(player.x);
    int z = round(player.z);

    if (mapXZ[x][z] == 0)
    {
        printf("Morreu");
        initPlayer();
    }



    if (leftPressed && !turningRight)
    {
        turningLeft = true;
    }

    if (rightPressed && !turningLeft)
    {
        turningRight = true;
    }

    if (turningLeft)
    {
        turning -= 10.0;
        roty -= 10.0;
        if (turning == -90)
        {
            turningLeft = false;
            turning = 0;
        }
    }

    if (turningRight)
    {
        turning += 10.0;
        roty += 10.0;
        if (turning == 90)
        {
            turningRight = false;
            turning = 0;
        }
    }


    if (upPressed || downPressed)
    {

        speedX = 0.05 * sin(roty*PI/180);
        speedZ = -0.05 * cos(roty*PI/180);

        if (upPressed)
        {
            player.x += speedX;
            player.z += speedZ;
        }
        else
        {
            player.x -= speedX;
            player.z -= speedZ;
        }

    }
}

/**
Render scene
*/
void mainRender()
{
    updateState();
    renderScene();
    glFlush();
    glutPostRedisplay();
    Sleep(30);
}


/**
Glut idle funtion
*/
void mainIdle()
{
    /**
    Set the active window before send an glutPostRedisplay call
    so it wont be accidently sent to the glui window
    */

    glutSetWindow(mainWindowId);
    glutPostRedisplay();
}

int main(int argc, char **argv)
{
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
