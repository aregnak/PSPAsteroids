#include <complex.h>
#include <math.h>

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
#define MAX_BULLETS 100

#define printf pspDebugScreenPrintf // don't need stdlib anyway

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

    // sceGuDepthFunc(GU_GEQUAL); //Depth buffer is reversed, so GEQUAL instead of LEQUAL
    // sceGuEnable(GU_DEPTH_TEST); //Enable depth testing

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
    sceGuClearColor(0x00000000); // background color
    sceGuClear(GU_COLOR_BUFFER_BIT);
    //sceGuClear(GU_DEPTH_BUFFER_BIT);
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
    float angle;
    
} Triangle;

void drawTriangle(Triangle* t)
{
    // allocate enough memory for 4 verticles
    Vertex* verts = (Vertex*)sceGuGetMemory(4 * sizeof(Vertex));

    // Calculate the half-width and half-height for centering
    float halfW = t->w / 2;
    float halfH = t->h / 2;

    // Define the vertices relative to the center of the triangle
    float vx[4] = {-halfW, halfW, 0, -halfW}; // x-coordinates
    float vy[4] = {-halfH, -halfH, halfH, -halfH}; // y-coordinates

    // Apply rotation transformation to each vertex
    float cosA = cosf(t->angle); // Precompute cosine
    float sinA = sinf(t->angle); // Precompute sine

    for (int i = 0; i < 4; i++)
    {
        // Rotate the vertex
        float xRot = vx[i] * cosA - vy[i] * sinA;
        float yRot = vx[i] * sinA + vy[i] * cosA;

        // Translate the vertex to the triangle's position
        verts[i].x = (short)(t->x + xRot);
        verts[i].y = (short)(t->y + yRot);
    }
    
    sceGuColor(0xFFFFFFFF); // colors are ABGR
    sceGuDrawArray(GU_LINE_STRIP, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 4, 0, verts);
}

typedef struct Bullet
{
    float x, y;
    float angle;
    float speed;
    short int active;

} Bullet;

Bullet pew[100] = {0.f, 0.f, 0.f, 1.f};

void initBullet()
{
    for (int i = 0; i < 100; i++)
    {
        pew[i].active = 0;
    }  
}

void drawBullet(Bullet *b)
{
    Vertex* p = (Vertex*)sceGuGetMemory(sizeof(Vertex));

    p[0].x = b->x;
    p[0].y = b->y;
    
    sceGuColor(0xFFFFFFFF); // colors are ABGR
    sceGuDrawArray(GU_POINTS, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 1, 0, p);
}

void moveBullet(Bullet *b)
{
    b->x += cosf(b->angle) * b->speed;
    b->y += sinf(b->angle) * b->speed;
}

void updateAndDrawBullets()
{
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (pew[i].active)
        {
            // Move the bullet
            moveBullet(&pew[i]);

            // Draw the bullet
            drawBullet(&pew[i]);

            if (pew[i].x < 0 || pew[i].x > SCREEN_WIDTH ||
                pew[i].y < 0 || pew[i].y > SCREEN_HEIGHT)
            {
                pew[i].active = 0;
            }
        }
    }
}


void handleArea(float *playerx, float *playery)
{
    if (*playerx <= -22.f)
    {
        *playerx = 480.f;    
    }
    else if (*playerx >= (float)SCREEN_WIDTH)
    {
        *playerx = 0.f;    
    }
    else if (*playery <= -36.f)
    {
        *playery = 272.f;    
    }
    else if (*playery >= (float)SCREEN_HEIGHT)
    {
        *playery = 0.f;    
    }
}

void handleSpeed(float *accx, float *accy)
{
    float maxAcc = 170.0f; // Maximum allowed acceleration
    if (*accx > maxAcc) *accx = maxAcc;
    if (*accx < -maxAcc) *accx = -maxAcc;
    if (*accy > maxAcc) *accy = maxAcc;
    if (*accy < -maxAcc) *accy = -maxAcc;
}

int main() {
    // Make exiting with the home button possible
    setup_callbacks();

    SceCtrlData pad;
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

    pspDebugScreenInit();


    Triangle player = {100.f, 50.f, 20.f, 34.f};

    
    initBullet();

    // Setup the library used for rendering
    initGu();

    // default, not moving
    float accx = 128.f;
    float accy = 128.f;
    float velx = 0, vely = 0;

    running = 1;
    while(running){
        startFrame();
        
        pspDebugScreenSetXY(0, 2);
        sceCtrlReadBufferPositive(&pad, 1); 

        accx -= (pad.Lx  - 128) / 20;
        accy -= (pad.Ly  - 128) / 20;


        velx = (accx - pad.Lx) / 50; // immitate acceleration and no gravity  
        vely = (accy - pad.Ly) / 50;                                          
                
        player.x -= velx; 
        player.y -= vely; 

        handleSpeed(&accx, &accy);
        
        handleArea(&player.x, &player.y);
        
        updateAndDrawBullets();

        if (pad.Buttons != 0)
        {
            if (pad.Buttons & PSP_CTRL_LTRIGGER)
            {
                player.angle -= 0.07f;
            }
            if (pad.Buttons & PSP_CTRL_RTRIGGER)
            {
                player.angle += 0.07f;
            }
            if (pad.Buttons & PSP_CTRL_CROSS)
            {
                for (int i = 0; i < MAX_BULLETS; i++)
                {
                    if (!pew[i].active)
                    {
                        // Spawn a new bullet
                        pew[i].x = player.x;
                        pew[i].y = player.y;
                        pew[i].angle = player.angle;
                        pew[i].speed = 3.0f; // Set bullet speed
                        pew[i].active = 1;   // Mark as active
                        break; 
                    }
                }
            }
        }      


        printf("Analog X = %3d, ", pad.Lx);
        printf("Analog Y = %3d \n", pad.Ly);
        
        printf("player x = %.4f\n", player.x);
        printf("player y = %.4f\n", player.y);

        printf("vel x = %.4f\n", accx);
        printf("vel y = %.4f\n", accy);

        drawTriangle(&player);

        endFrame();
    }

    return 0;
}
