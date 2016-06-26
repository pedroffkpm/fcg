#include <GL/gl.h>
#include <GL/glut.h>

#include "bitmap.h"
#include <stdio.h>
#include <windows.h>

#include "maps.h"
boolean opened = false;
boolean playerLoaded = false;
int enemiesLoaded = 0;
int mapXZ[32][32] = { {0} };


GLenum      type;            /* Texture type */
GLuint      grass, grass_cracked, grass_hole;         /* Texture object */


void openMap() {

    bits = LoadDIBitmap("map.bmp", &info);

	if (bits == (GLubyte *)0) {
		printf ("Error loading map!\n\n");
		return;
	}

	int xpos = 0;
    int zpos = (int)info->bmiHeader.biHeight - 1;

	int i = info->bmiHeader.biWidth * info->bmiHeader.biHeight;
    glColor4f(1.0f,1.0f,1.0f,1.0f);

    for(ptr = bits; i > 0; i--, ptr += 3) {
        int color = (ptr[2] << 16) + (ptr[1] << 8) + ptr[0];
        if (color == 0x00ff00) {
            mapXZ[xpos][zpos] = 1;
        }


        xpos++;
        if(xpos ==(int)info->bmiHeader.biWidth) {
            xpos = 0;
            zpos--;
        }
    }

    bits = LoadDIBitmap("map2.bmp", &info);

	if (bits == (GLubyte *)0) {
		printf ("Error loading map!\n\n");
		return;
	}

	xpos = 0;
    zpos = (int)info->bmiHeader.biHeight - 1;

	i = info->bmiHeader.biWidth * info->bmiHeader.biHeight;

    for(ptr = bits; i > 0; i--, ptr += 3) {
        int color = (ptr[2] << 16) + (ptr[1] << 8) + ptr[0];
        if (color == 0x8000ff) {
            mapXZ[xpos][zpos] = 2;
        }
        if (color == 0x804000) {
            mapXZ[xpos][zpos] = 3;
        }
        if (color == 0x0000ff) {
            if(playerLoaded == false){
                mapXZ[xpos][zpos] = 4;
                playerLoaded = true;
            }
            else printf("Você tentou carregar mais de um jogador!\n");
        }
        if (color == 0xff0000) {
            if(enemiesLoaded < 4) {
                mapXZ[xpos][zpos] = 5;
                enemiesLoaded++;
            }
            else printf("Você tentou carregar mais de 4 inimigos!\n");
        }



        xpos++;
        if(xpos ==(int)info->bmiHeader.biWidth) {
            xpos = 0;
            zpos--;
        }
    }


}

void renderFloor() {

    glShadeModel(GL_SMOOTH);
	glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glPushMatrix();

    glColor4f(1.0f,1.0f,1.0f,1.0f);

    for(int x = 0; x < 32; x++) {
        for (int z = 0; z < 32; z++) {
        if (mapXZ[x][z] == 1 || mapXZ[x][z] == 4 || mapXZ[x][z] == 5) {
            glBindTexture(type, grass);

            glBegin(GL_QUADS);
                glTexCoord2f(1.0f, 0.0f);   // coords for the texture
                glNormal3f(0.0f,1.0f,0.0f);
                glVertex3f(x - 0.5f, 0.0f, z + 0.5f);

                glTexCoord2f(0.0f, 0.0f);  // coords for the texture
                glNormal3f(0.0f,1.0f,0.0f);
                glVertex3f(x + 0.5f, 0.0f, z + 0.5f);

                glTexCoord2f(0.0f, 1.0f);  // coords for the texture
                glNormal3f(0.0f,1.0f,0.0f);
                glVertex3f(x + 0.5f, 0.0f, z - 0.5f);

                glTexCoord2f(1.0f, 1.0f);  // coords for the texture
                glNormal3f(0.0f,1.0f,0.0f);
                glVertex3f(x - 0.5f, 0.0f, z - 0.5f);

            glEnd();
        }
        if (mapXZ[x][z] == 2) {
            glBindTexture(type, grass_hole);

            glBegin(GL_QUADS);
                glTexCoord2f(1.0f, 0.0f);   // coords for the texture
                glNormal3f(0.0f,1.0f,0.0f);
                glVertex3f(x - 0.5f, 0.0f, z + 0.5f);

                glTexCoord2f(0.0f, 0.0f);  // coords for the texture
                glNormal3f(0.0f,1.0f,0.0f);
                glVertex3f(x + 0.5f, 0.0f, z + 0.5f);

                glTexCoord2f(0.0f, 1.0f);  // coords for the texture
                glNormal3f(0.0f,1.0f,0.0f);
                glVertex3f(x + 0.5f, 0.0f, z - 0.5f);

                glTexCoord2f(1.0f, 1.0f);  // coords for the texture
                glNormal3f(0.0f,1.0f,0.0f);
                glVertex3f(x - 0.5f, 0.0f, z - 0.5f);

            glEnd();
        }
        if (mapXZ[x][z] == 3) {
            glBindTexture(type, grass_cracked);

            glBegin(GL_QUADS);
                glTexCoord2f(1.0f, 0.0f);   // coords for the texture
                glNormal3f(0.0f,1.0f,0.0f);
                glVertex3f(x - 0.5f, 0.0f, z + 0.5f);

                glTexCoord2f(0.0f, 0.0f);  // coords for the texture
                glNormal3f(0.0f,1.0f,0.0f);
                glVertex3f(x + 0.5f, 0.0f, z + 0.5f);

                glTexCoord2f(0.0f, 1.0f);  // coords for the texture
                glNormal3f(0.0f,1.0f,0.0f);
                glVertex3f(x + 0.5f, 0.0f, z - 0.5f);

                glTexCoord2f(1.0f, 1.0f);  // coords for the texture
                glNormal3f(0.0f,1.0f,0.0f);
                glVertex3f(x - 0.5f, 0.0f, z - 0.5f);

            glEnd();
        }



        }
    }

	glDisable(GL_TEXTURE_2D);


	glPopMatrix();

}
