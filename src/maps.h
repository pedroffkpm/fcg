extern BITMAPINFO	*info;           /* Bitmap information */
extern GLubyte	    *bits;           /* Bitmap RGB pixels */
extern GLubyte     *ptr;            /* Pointer into bit buffer */
extern GLenum      type;            /* Texture type */
extern GLuint      grass, grass_cracked, grass_hole;         /* Texture object */
extern int mapXZ[32][32];

extern void openMap();
