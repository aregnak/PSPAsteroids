#include <complex.h>
#include <pspkernel.h>
#include <pspgu.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include <pspdebug.h>

PSP_MODULE_INFO("shape", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_VFPU | THREAD_ATTR_USER);

#define BUFFER_WIDTH 512
#define BUFFER_HEIGHT 272
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT BUFFER_HEIGHT

#define printf pspDebugScreenPrintf

char list[0x20000] __attribute__((aligned(64)));
int running;

int exit_callback(int arg1, int arg2, void *common) 
{
    running = 0;
    return 0;
}

int callback_thread(SceSize args, void *argp) 
{
    int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
    sceKernelRegisterExitCallback(cbid);
    sceKernelSleepThreadCB();
    return 0;
}

int setup_callbacks(void) 
{
    int thid = sceKernelCreateThread("update_thread", callback_thread, 0x11, 0xFA0, 0, 0);
    if(thid >= 0)
        sceKernelStartThread(thid, 0, 0);
    return thid;
}

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
    sceGuEnable(GU_SCISSOR_TEST);
    sceGuScissor(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    //Set some stuff
    sceGuDepthRange(65535, 0); //Use the full buffer for depth testing - buffer is reversed order

    sceGuDepthFunc(GU_GEQUAL); //Depth buffer is reversed, so GEQUAL instead of LEQUAL
    sceGuEnable(GU_DEPTH_TEST); //Enable depth testing

    sceGuFinish();
    sceGuDisplay(GU_TRUE);
}

void endGu()
{
    sceGuDisplay(GU_FALSE);
    sceGuTerm();
}

void startFrame()
{
    sceGuStart(GU_DIRECT, list);
    sceGuClearColor(0x00000000); // White background
    sceGuClear(GU_COLOR_BUFFER_BIT);
}

void endFrame()
{
    sceGuFinish();
    sceGuSync(0, 0);
    sceDisplayWaitVblankStart();
    sceGuSwapBuffers();
}

typedef struct Vertex
{
    unsigned short u, v;
    short x, y, z;
} Vertex;

typedef struct Triangle
{
    float x, y;
    float w, h;
    
} Triangle;

void drawTriangle(Triangle* t)
{
    Vertex* verts = (Vertex*)sceGuGetMemory(4 * sizeof(Vertex));

    verts[0].x = t->x;
    verts[0].y = t->y;

    verts[1].x = t->x + t->w;
    verts[1].y = t->y; 

    verts[2].x = t->x + t->w / 2;
    verts[2].y = t->y + t->h; 

    verts[3].x = t->x;
    verts[3].y = t->y;
    
    sceGuColor(0xFFFFFFFF); // Red, colors are ABGR
    sceGuDrawArray(GU_LINE_STRIP, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 4, 0, verts);
}

int main() {
    // Make exiting with the home button possible
    setup_callbacks();

    SceCtrlData pad;
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

    pspDebugScreenInit();


    Triangle player = {100.f, 50.f, 20.f, 34.f};
    
    // Setup the library used for rendering
    initGu();

    running = 1;
    while(running){
        startFrame();
        
        pspDebugScreenSetXY(0, 2);
        sceCtrlReadBufferPositive(&pad, 1); 

        player.x -= (128 - pad.Lx) / 20; 

        printf("Analog X = %3d, ", pad.Lx);
        printf("Analog Y = %3d \n", pad.Ly);
        
        printf("player x=%.4f\n", player.x);
        printf("player y=%.4f\n", player.y);

        if (pad.Buttons != 0)
        {
            if (pad.Buttons & PSP_CTRL_LEFT)
            {
                player.x += 5;
            }
            if (pad.Buttons & PSP_CTRL_RIGHT)
            {
                player.x -= 5;
            }
        }

        drawTriangle(&player);

        endFrame();
    }

    return 0;
}
