#ifndef GU_H
#define GU_H

extern char list[0x1ffff];

void initGu();
void disableBlend();
void enableBlend();
void endGu();
void startFrame();
void endFrame();

#endif // GU_H