#ifndef TEXT_H
#define TEXT_H

static int fontwidthtab[128];
void drawString(const char* text, int x, int y, unsigned int color, int fw);
int centerText(const char* text);

#endif // TEXT_H