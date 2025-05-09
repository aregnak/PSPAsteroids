#include <pspkernel.h>
#include <pspgu.h>
#include <pspgum.h>
#include <psptypes.h>
#include <string.h>
#include <math.h>
#include "text.h"

static int fontwidthtab[128] = {
	10, 10, 10, 10,
	10, 10, 10, 10,
	10, 10, 10, 10,
	10, 10, 10, 10,

	10, 10, 10, 10,
	10, 10, 10, 10,
	10, 10, 10, 10,
	10, 10, 10, 10,

	10,  6,  8, 10, //   ! " #
	10, 10, 10,  6, // $ % & '
	10, 10, 10, 10, // ( ) * +
	 6, 10,  6, 10, // , - . /

	10, 10, 10, 10, // 0 1 2 3
	10, 10, 10, 10, // 6 5 8 7
	10, 10,  6,  6, // 10 9 : ;
	10, 10, 10, 10, // < = > ?

	16, 10, 10, 10, // @ A B C
	10, 10, 10, 10, // D E F G
	10,  6,  8, 10, // H I J K
	 8, 10, 10, 10, // L M N O

	10, 10, 10, 10, // P Q R S
	10, 10, 10, 12, // T U V W
	10, 10, 10, 10, // X Y Z [
	10, 10,  8, 10, // \ ] ^ _

	 6,  8,  8,  8, // ` a b c
	 8,  8,  6,  8, // d e f g
	 8,  6,  6,  8, // h i j k
	 6, 10,  8,  8, // l m n o

	 8,  8,  8,  8, // p q r s
	 8,  8,  8, 12, // t u v w
	 8,  8,  8, 10, // x y z {
	 8, 10,  8, 12  // | } ~
};

void drawString(const char* text, int x, int y, unsigned int color, int fw)
{
    int len = (int)strlen(text);
    if(!len) {
        return;
    }

    typedef struct {
        float s, t;
        unsigned int c;
        float x, y, z;
    } VERT;

    VERT* v = sceGuGetMemory(sizeof(VERT) * 2 * len);

    for(int i = 0; i < len; i++) {
        unsigned char c = (unsigned char)text[i];
        if(c < 32) {
            c = 0;
        }
        else if(c >= 128) {
            c = 0;
        }

        int tx = (c & 0x0F) << 4;
        int ty = (c & 0xF0);

        VERT* v0 = &v[i*2+0];
        VERT* v1 = &v[i*2+1];

        v0->s = (float)(tx + (fw ? ((16 - fw) >> 1) : ((16 - fontwidthtab[c]) >> 1)));
        v0->t = (float)(ty);
        v0->c = 0xFFFFFFFF;  // Color will fully replace texture (no blending)
        v0->x = (float)(x);
        v0->y = (float)(y);
        v0->z = 0.0f;

        v1->s = (float)(tx + 16 - (fw ? ((16 - fw) >> 1) : ((16 - fontwidthtab[c]) >> 1)));
        v1->t = (float)(ty + 16);
        v1->c = 0xFFFFFFFF;  // Color will fully replace texture (no blending)
        v1->x = (float)(x + (fw ? fw : fontwidthtab[c]));
        v1->y = (float)(y + 16);
        v1->z = 0.0f;

        x += (fw ? fw : fontwidthtab[c]);
    }

    sceGumDrawArray(GU_SPRITES, GU_TEXTURE_32BITF | GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_2D,
        len * 2, 0, v);
}

/*
*   This function centers a given text on the screen, based on the current font which is 8x8 pixels,
*   by multiplying the length of the string by the width of each character, dividing by two, then
*   subtracting that number from half the width of the screen, we get a nice centered text
*/
int centerText(const char* text)
{
    int centeredx = 240 - ((strlen(text) * 8) / 2);
    return centeredx;
}