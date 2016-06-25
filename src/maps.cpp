#include <GL/gl.h>
#include <GL/glut.h>

#include "bitmap.h"
#include <stdio.h>
#include <windows.h>

#include "maps.h"

float planeSize = 8.0f;

void renderFloor() {
	// set things up to render the floor according to bmp
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

    glTranslatef(-(float)planeSize/2.0f, 0.0f, -(float)planeSize/2.0f);


    glColor4f(1.0f,1.0f,1.0f,1.0f);
    int xQuads = 40;
    int yQuads = 40;
    int zQuads = 40;

    for(ptr = bits; i > 0; i--, ptr += 3) {
        int color = (ptr[2] << 16) + (ptr[1] << 8) + ptr[0];
        if (color == 0x00ff00) {
            glBegin(GL_QUADS);
                glTexCoord2f(1.0f, 0.0f);   // coords for the texture
                glNormal3f(0.0f,1.0f,0.0f);
                glVertex3f(xpos * (float)planeSize/xQuads, 0.0f, (zpos+1) * (float)planeSize/zQuads);

                glTexCoord2f(0.0f, 0.0f);  // coords for the texture
                glNormal3f(0.0f,1.0f,0.0f);
                glVertex3f((xpos+1) * (float)planeSize/xQuads, 0.0f, (zpos+1) * (float)planeSize/zQuads);

                glTexCoord2f(0.0f, 1.0f);  // coords for the texture
                glNormal3f(0.0f,1.0f,0.0f);
                glVertex3f((xpos+1) * (float)planeSize/xQuads, 0.0f, zpos * (float)planeSize/zQuads);

                glTexCoord2f(1.0f, 1.0f);  // coords for the texture
                glNormal3f(0.0f,1.0f,0.0f);
                glVertex3f(xpos * (float)planeSize/xQuads, 0.0f, zpos * (float)planeSize/zQuads);

                glEnd();
        }

/**    for (int i = 0; i < xQuads; i++) {
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
                glVertex3f(i * (float)planeSize/xQuads, 0.0f, j * (float)planeSize/zQuads);*/



        xpos++;
        if(xpos ==(int)info->bmiHeader.biWidth) {
            xpos = 0;
            zpos--;
        }
    }

	glDisable(GL_TEXTURE_2D);


	glPopMatrix();
}
