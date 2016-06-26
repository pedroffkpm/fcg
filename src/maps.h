extern BITMAPINFO	*info;           /* Bitmap information */
extern GLubyte	    *bits;           /* Bitmap RGB pixels */
extern GLubyte     *ptr;            /* Pointer into bit buffer */
extern GLenum      type;            /* Texture type */
extern GLuint      grass, grass_cracked, grass_hole, dirt, water;         /* Texture object */
extern int mapXZ[32][32];
extern float heightXZ[32][32];
extern int enemiesLoaded;

extern void openMap();
extern void renderFloor();
extern void renderWater();
