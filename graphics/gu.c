#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspgu.h>

#include "gu.h"
#include "../common/config.h"
#include "../src/text/font.h"

char list[0x1ffff] __attribute__((aligned(64)));

void initGu()
{
    sceGuInit();

    //Set up buffers
    sceGuStart(GU_DIRECT, list);
    sceGuDrawBuffer(GU_PSM_8888,(void*)0,BUFFER_WIDTH);
    sceGuDispBuffer(SCREEN_WIDTH,SCREEN_HEIGHT,(void*)0x88000,BUFFER_WIDTH);
    sceGuDepthBuffer((void*)0x110000,BUFFER_WIDTH);

    //Set up viewport
    sceGuOffset(2048 - (SCREEN_WIDTH / 2), 2048 - (SCREEN_HEIGHT / 2));
    sceGuViewport(2048, 2048, SCREEN_WIDTH, SCREEN_HEIGHT);
    sceGuDepthRange(0xc350, 0x2710);
    sceGuEnable(GU_SCISSOR_TEST);
    sceGuScissor(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    // Additional GU settings from second block
    sceGuDisable(GU_DEPTH_TEST);
    sceGuEnable(GU_BLEND);
    sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
    sceGuShadeModel(GU_SMOOTH);
    sceGuEnable(GU_TEXTURE_2D);
    sceGuTexMode(GU_PSM_8888, 0, 0, 0);
    sceGuTexImage(0, 256, 128, 256, font);
    sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGBA);
    sceGuTexEnvColor(0x0);
    sceGuTexOffset(0.0f, 0.0f);
    sceGuTexScale(1.0f / 256.0f, 1.0f / 128.0f);
    sceGuTexWrap(GU_REPEAT, GU_REPEAT);
    sceGuTexFilter(GU_NEAREST, GU_NEAREST);

    sceGuFinish();
    sceGuSync(GU_SYNC_FINISH, GU_SYNC_WHAT_DONE);
    sceGuDisplay(GU_TRUE);
}

// this function is needed to render entities properly
// otherwise, the blend option conflicts with font rendering
void disableBlend()
{
    sceGuDisable(GU_BLEND);
}

// Same here, this is needed to enable font rendering.
void enableBlend()
{
    sceGuEnable(GU_BLEND);
    sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
}

void endGu()
{
    sceGuDisplay(GU_FALSE);
    sceGuTerm();
}

void startFrame()
{
    sceGuStart(GU_DIRECT, list);
    sceGuClearColor(0x00000000); // background color
    sceGuClear(GU_COLOR_BUFFER_BIT);
    //sceGuClear(GU_DEPTH_BUFFER_BIT);
}

void endFrame()
{
    sceGuFinish();
    sceGuSync(GU_SYNC_FINISH, GU_SYNC_WHAT_DONE);
    sceDisplayWaitVblankStart();
    sceGuSwapBuffers();
}