#include "models.h"
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


