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
//bitmap class to load bitmaps for textures
#include "bitmap.h"
#pragma comment(lib, "OpenAL32.lib")
#pragma comment(lib, "alut.lib")
#define PI 3.14159265
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
void initEnemies();

int mainWindowId = 0;


int i;                       /* Looping var */
BITMAPINFO	*info;           /* Bitmap information */
GLubyte	    *bits;           /* Bitmap RGB pixels */
GLubyte     *ptr;            /* Pointer into bit buffer */
GLubyte	    *rgba;           /* RGBA pixel buffer */
GLubyte	    *rgbaptr;        /* Pointer into RGBA buffer */
GLubyte     temp;            /* Swapping variable */

// parte de código extraído de "texture.c" por Michael Sweet (OpenGL SuperBible)
// texture buffers and stuff

float gravity = 0.004;

float backgroundColor[4] = {0.529f,0.807f,0.980f,1.0f};

struct object
{
    GLMmodel *model;
    float x;
    float z;
    float y;
    float speedX = 0.0f;
    float speedZ = 0.0f;
    float roty = 0.0f;
    bool alive = true;
} enemies[4], player;

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
        gluLookAt(player.x + 0.05f * sin(player.roty*PI/180), 0.9, player.z - 0.05f * cos(player.roty*PI/180),
                  player.x + sin(player.roty*PI/180), 0.6, player.z - cos(player.roty*PI/180),
                  0.0,1.0, 0.0);
    }
    if(cam == 1)
    {
        gluLookAt(player.x - 3.0f * sin(player.roty*PI/180), player.y + 1.0f, player.z + 3.0f * cos(player.roty*PI/180),
                  player.x + sin(player.roty*PI/180), 0.9f, player.z - cos(player.roty*PI/180),
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
    glEnable(GL_LIGHT0);

    GLfloat light_ambient[] = { backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3] };
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_position0[] = { 4.0, 4.0, 3.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
}

void initEnemies()
{
    int cont = 0;
    for (int x = 0; x < 32; x++)
        for (int z = 0; z < 32; z++)
            if (mapXZ[x][z] == 5)
            {
                enemies[cont].x = x;
                enemies[cont].y = 0.48f;
                enemies[cont].z = z;
                enemies[cont].speedX = 0.07f;
                enemies[cont].speedZ = -0.07f;
                cont++;
            }

    for (int i = 0; i< enemiesLoaded; i++)
    {
        printf("Loading enemy %d.. \n", i);

        bool modelLoaded = false;

        modelLoaded = C3DObject_Load_New("../../models/OogieBoogie.obj", &enemies[i].model);

        if (!modelLoaded)
            printf("Erro ao carregar modelo!\n");
        else printf("Enemy Model ok. \n \n \n");
    }
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

    player.y = 0.49f;
    player.alive = false;
    printf("Loading player.. \n");

    bool modelLoaded = false;

    modelLoaded = C3DObject_Load_New("../../models/LEGO_man.obj", &player.model);


    if (!modelLoaded)
        printf("Erro ao carregar modelo!\n");
    else {
            printf("Player Model ok. \n \n \n");
            player.alive = true;
        }
}

/**
Initialize the texture using the library bitmap
*/
void initTextures()
{
    printf ("\nLoading grass texture..\n");
    // Load a texture object (256x256 true color)
    bits = LoadDIBitmap("../../res/grass2.bmp", &info);
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
    bits = LoadDIBitmap("../../res/grass_cracked3.bmp", &info);
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

    printf ("\nLoading dirt texture..\n");
    // Load a texture object (256x256 true color)
    bits = LoadDIBitmap("../../res/dirt.bmp", &info);
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
    glGenTextures(1, &dirt);
    glBindTexture(type, dirt);

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

    printf ("\nLoading water texture..\n");
    // Load a texture object (256x256 true color)
    bits = LoadDIBitmap("../../res/water.bmp", &info);
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
    glGenTextures(1, &water);
    glBindTexture(type, water);

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

void checkCollisions()
{
    int colisorX, colisorZ;
    for (i = 0; i<enemiesLoaded; i++)
    {
        colisorX = round(enemies[i].x);
        colisorZ = round(enemies[i].z);

        if(round(player.x) == colisorX && round(player.z) == colisorZ)
            player.alive = false;

        if (mapXZ[colisorX][colisorZ] == 0)
            enemies[i].alive = false;
    }
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

    initEnemies();

    initLight();

    //enableFog();
}

void enableFog(void)
{
    float fog_colour[4] = {0.5,0.5,0.5,1.0};
    glEnable(GL_FOG);
    glFogf(GL_FOG_MODE,GL_EXP2);
    glFogfv(GL_FOG_COLOR,fog_colour);
    glFogf(GL_FOG_DENSITY,0.07);
    glHint(GL_FOG_HINT,GL_NICEST);
    glFogf(GL_FOG_START, 700);
    glFogf(GL_FOG_END,1000);
}

void renderScene()
{
    glClearColor(backgroundColor[0],backgroundColor[1],backgroundColor[2],backgroundColor[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // limpar o depth buffer

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    updateCam();

    int cont;
    for (cont=0; cont<enemiesLoaded; cont++)
    {
        if(enemies[cont].alive)
        {
            glPushMatrix();
            glTranslatef(GLfloat(enemies[cont].x), GLfloat (enemies[cont].y), GLfloat(enemies[cont].z));
            glScalef(0.55,0.55,0.55);
            glRotatef(-(enemies[cont].roty+180), 0.0, 1.0, 0.0);
            glmDraw(enemies[cont].model, GLM_SMOOTH | GLM_MATERIAL);
            glPopMatrix();
        }
    }

    glPushMatrix();
    glTranslatef(GLfloat(player.x), GLfloat(player.y), GLfloat(player.z));
    glScalef(0.5, 0.5, 0.5);
    glRotatef(-(player.roty+180), 0.0, 1.0, 0.0);
    glmDraw(player.model, GLM_SMOOTH | GLM_MATERIAL);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(GLfloat(4), GLfloat(player.y), GLfloat(4));
    glScalef(0.5, 0.5, 0.5);
    glmDraw(player.model, GLM_SMOOTH | GLM_MATERIAL);
    glPopMatrix();

    // binds the bmp file already loaded to the OpenGL parameters


    renderFloor();
    renderWater();
}


void moveEnemies()
{
    int decision;
    for (i = 0; i<enemiesLoaded; i++)
    {
        if(enemies[i].alive)
        {
            enemies[i].speedX = 0.07 * sin(enemies[i].roty*PI/180);
            enemies[i].speedZ = -0.07 * cos(enemies[i].roty*PI/180);
            decision = rand() % 20;  //decision vai ser  baseado na IA
            switch(decision)
            {
                break;
            case 1: //vira à esquerda
                enemies[i].roty -= 90.0;
                break;
            case 2: // vira à direita
                enemies[i].roty += 90.0;
            default: //demais casos, segue reto
                enemies[i].x += enemies[i].speedX;
                enemies[i].z += enemies[i].speedZ; //speedZ é negativa
                break;
            }
        }
    }

}

int floodFill(int floodMap[32][32], int x, int z, int target, int replacement)
{
    if (x >= 32 || z >= 32 || x < 0 || z < 0)
        return 0;

    if (floodMap[x][z] == replacement)
        return 0;

    if (floodMap[x][z] == target)
    {
        floodMap[x][z] = replacement;
        return 1 +
               floodFill(floodMap, x, z-1, target, replacement) +
               floodFill(floodMap, x+1, z, target, replacement) +
               floodFill(floodMap, x, z+1, target, replacement) +
               floodFill(floodMap, x-1, z, target, replacement);
    }

    return 0;
}

void updateAreas(int playerX, int playerZ)
{
    int newMap[32][32];

    for(int x = 0; x < 32; x++)
    {
        for (int z = 0; z < 32; z++)
        {
            if (mapXZ[x][z] == 1 || mapXZ[x][z] == 4 || mapXZ[x][z] == 5)
                newMap[x][z] = 1;
            else if (mapXZ[x][z] == 2 || mapXZ[x][z] == 3)
                newMap[x][z] = 2;
            else newMap[x][z] = 0;
        }
    }

    int tamArea;
    int maxArea = 0;
    int replacement = -1;
    int maxReplacement = -1;
    int numAreas = 0;

    for(int x = 0; x < 32; x++)
    {
        for (int z = 0; z < 32; z++)
        {
            if (newMap[x][z] == 1)
            {
                tamArea = floodFill(newMap, x, z, 1, replacement);
                numAreas++;
                if (tamArea > maxArea)
                {
                    maxArea = tamArea;
                    maxReplacement = replacement;
                }
                replacement--;
            }
        }
    }

    if (numAreas > 1)
    {
        for(int x = 0; x < 32; x++)
        {
            for (int z = 0; z < 32; z++)
            {
                if (newMap[x][z] != maxReplacement && newMap[x][z] < 0)
                {
                    mapXZ[x][z] = 0;
                }
                else
                {
                    if (newMap[x][z] == 2 && z - 1 > 0 && z + 1 < 32 && x - 1 > 0 && x + 1 < 32)
                    {
                        if (newMap[x][z-1] != maxReplacement && newMap[x][z-1] < 0)
                            if (newMap[x][z+1] != maxReplacement && newMap[x][z+1] < 0)
                                if (newMap[x-1][z] != maxReplacement && newMap[x-1][z] < 0)
                                    if (newMap[x+1][z] != maxReplacement && newMap[x+1][z] < 0)
                                        mapXZ[x][z] = 0;
                    }
                }
            }
        }
    }

}

void createCrack(int x, int z)
{
    double fX = sin(player.roty*PI/180);
    double fZ = -1 * cos(player.roty*PI/180);

    int facingX;
    int facingZ;

    if (fX < -0.5)
        facingX = -1;
    else if (fX > 0.5)
        facingX = 1;
    else facingX = 0;

    if (fZ < -0.5)
        facingZ = -1;
    else if (fZ > 0.5)
        facingZ = 1;
    else facingZ = 0;

    for (int i = 1; i < 31; i++)
    {
        if (x + i*facingX >= 32 && z + i*facingZ >= 32)
        {
            updateAreas(x, z);
            return;
        }
        else
        {
            if (mapXZ[x + i*facingX][z + i*facingZ] == 0 || mapXZ[x + i*facingX][z + i*facingZ] == 2 || mapXZ[x + i*facingX][z + i*facingZ] == 3)
            {
                updateAreas(x, z);
                return;
            }
            else
            {
                mapXZ[x + i*facingX][z + i*facingZ] = 3;
            }
        }

    }
    updateAreas(x, z);
    return;
}

void updateState()
{
    int x = round(player.x);
    int z = round(player.z);

    if (spacePressed && (!turningRight && !turningLeft))
    {
        if (mapXZ[x][z] == 2)
        {
            createCrack(x, z);
        }
        spacePressed = false;
    }
    else spacePressed = false;

    checkCollisions();

    if (mapXZ[x][z] == 0)
    {
        player.alive = false;
    }

    if (spacePressed && (!turningRight && !turningLeft))
    {
        if (mapXZ[x][z] == 2)
        {
            createCrack(x, z);
        }
        spacePressed = false;
    }
    else spacePressed = false;

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
        player.roty -= 10.0;
        if (turning == -90)
        {
            turningLeft = false;
            turning = 0;
        }
    }

    if (turningRight)
    {
        turning += 10.0;
        player.roty += 10.0;
        if (turning == 90)
        {
            turningRight = false;
            turning = 0;
        }
    }


    if (upPressed || downPressed)
    {

        player.speedX = 0.1 * sin(player.roty*PI/180);
        player.speedZ = -0.1 * cos(player.roty*PI/180);

        if (upPressed)
        {
            player.x += player.speedX;
            player.z += player.speedZ;
        }
        else
        {
            player.x -= player.speedX;
            player.z -= player.speedZ;
        }

    }
    moveEnemies();
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
