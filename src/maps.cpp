#include <GL/gl.h>
#include <GL/glut.h>

#include "bitmap.h"
#include <stdio.h>
#include <windows.h>

#include "maps.h"
boolean opened = false;
int mapXZ[32][32] = { {0} };

void openFloor() {

    bits = LoadDIBitmap("map.bmp", &info);

	if (bits == (GLubyte *)0) {
		printf ("Error loading map!\n\n");
		return;
	}

	int xpos = 0;
    int zpos = (int)info->bmiHeader.biHeight - 1;

	int i = info->bmiHeader.biWidth * info->bmiHeader.biHeight;
	glShadeModel(GL_SMOOTH);
	glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glPushMatrix();


    glColor4f(1.0f,1.0f,1.0f,1.0f);

    for(ptr = bits; i > 0; i--, ptr += 3) {
        int color = (ptr[2] << 16) + (ptr[1] << 8) + ptr[0];
        if (color == 0x00ff00) {
            mapXZ[xpos][zpos] = 1;
            glBegin(GL_QUADS);
                glTexCoord2f(1.0f, 0.0f);   // coords for the texture
                glNormal3f(0.0f,1.0f,0.0f);
                glVertex3f(xpos - 0.5f, 0.0f, zpos + 0.5f);

                glTexCoord2f(0.0f, 0.0f);  // coords for the texture
                glNormal3f(0.0f,1.0f,0.0f);
                glVertex3f(xpos + 0.5f, 0.0f, zpos + 0.5f);

                glTexCoord2f(0.0f, 1.0f);  // coords for the texture
                glNormal3f(0.0f,1.0f,0.0f);
                glVertex3f(xpos + 0.5f, 0.0f, zpos - 0.5f);

                glTexCoord2f(1.0f, 1.0f);  // coords for the texture
                glNormal3f(0.0f,1.0f,0.0f);
                glVertex3f(xpos - 0.5f, 0.0f, zpos - 0.5f);

            glEnd();
        }


        xpos++;
        if(xpos ==(int)info->bmiHeader.biWidth) {
            xpos = 0;
            zpos--;
        }
    }

	glDisable(GL_TEXTURE_2D);


	glPopMatrix();

}

void renderFloor() {
	if (!opened) {
		openFloor();
		opened = true;
	}

    glShadeModel(GL_SMOOTH);
	glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glPushMatrix();

    glColor4f(1.0f,1.0f,1.0f,1.0f);

    for(int x = 0; x < 32; x++) {
        for (int z = 0; z < 32; z++) {
        if (mapXZ[x][z] == 1) {
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
