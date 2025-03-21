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

#define MAX_BULLETS 20
#define AST_VERTS 12
#define MAX_AST 10

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

void getTriPeak(Triangle* t, float *peakx, float *peaky)
{
    float localx = 0;
    float localy = t->h / 2;

    // Rotate the peak position based on the triangle's angle
    float cosA = cosf(t->angle);
    float sinA = sinf(t->angle);
    *peakx = t->x + (localx * cosA - localy * sinA);
    *peaky = t->y + (localx * sinA + localy * cosA);
}

typedef struct Asteroid
{
    float x, y;
    float w, h;
    float angle;
    // float velx = (rand() % 200) - 100;
    // float vely = (rand() % 200) - 100;
    char active;
} Asteroid;

Asteroid rock[MAX_AST] = {0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0};

void drawAsteroid(Asteroid* a)
{
    // allocate enough memory for 11 verticles for a nice looking asteroid
    Vertex* verts = (Vertex*)sceGuGetMemory(AST_VERTS * sizeof(Vertex));

    // Calculate the half-width and half-height for centering
    float halfW = a->w / 2;
    float halfH = a->h / 2;

    // Define the vertices relative to the center of the triangle
    float vx[AST_VERTS] = {
        -halfW * 0.8,
        0,
        halfW * 0.5,
        halfW * 0.60, 
        halfW, 
        halfW * 0.25, 
        halfW * 0.33, 
        -halfW * 0.65, 
        -halfW * 0.55, 
        -halfW, 
        -halfW * 0.6, 
        -halfW * 0.80}; // x-coordinates

    float vy[AST_VERTS] = {
        -halfH * 0.95, 
        -halfH * 0.5, 
        -halfH * 0.75, 
        0, 
        halfH * 0.4, 
        halfH * 0.5, 
        halfH, 
        halfH * 0.9, 
        halfH * 0.4, 
        0, 
        -halfH * 0.45, 
        -halfH * 0.95}; // y-coordinates

    // Apply rotation transformation to each vertex
    float cosA = cosf(a->angle); 
    float sinA = sinf(a->angle); 

    for (int i = 0; i < AST_VERTS; i++)
    {
        // Rotate the vertex
        float xRot = vx[i] * cosA - vy[i] * sinA;
        float yRot = vx[i] * sinA + vy[i] * cosA;

        // Translate the vertex to the triangle's position
        verts[i].x = (short)(a->x + xRot);
        verts[i].y = (short)(a->y + yRot);
    }
    
    sceGuColor(0xFFFFFFFF); // colors are ABGR
    sceGuDrawArray(GU_LINE_STRIP, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, AST_VERTS, 0, verts);
}

void initAsteroid()
{
    for (int i = 0; i < 3; i++)
    {
        rock[i].active = 1;     
    } 
}

void moveAsteroid()
{
    
}

void updateAsteroid()
{
    for (int i = 0; i < MAX_AST; i++)
    {
        if (rock[i].active)
        {
            moveAsteroid(&rock[i]);

            drawAsteroid(&rock[i]);
        }
    }
}

typedef struct Bullet
{
    float x, y;
    float angle;
    float speed;
    char active;

} Bullet;

// create global Bullet object
Bullet pew[MAX_BULLETS] = {0.f, 0.f, 0.f, 0.f, 0};

void initBullet()
{
    for (int i = 0; i < MAX_BULLETS; i++)
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

void updateBullets()
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
                // "remove" the bullet
                pew[i].active = 0;
            }
        }
    }
}

// loop around the screen if at the edges
void handleArea(float *x, float *y)
{
    if (*x <= -22.f)
    {
        *x = 480.f;    
    }
    else if (*x >= (float)SCREEN_WIDTH)
    {
        *x = 0.f;    
    }
    else if (*y <= -36.f)
    {
        *y = 272.f;    
    }
    else if (*y >= (float)SCREEN_HEIGHT)
    {
        *y = 0.f;    
    }
}

void handleSpeed(float *accx, float *accy)
{
    float maxAcc = 155.0f; // Maximum allowed acceleration
    if (*accx > maxAcc) *accx = maxAcc;
    if (*accx < maxAcc - 128) *accx = maxAcc - 128; //equalize positive and negative acceleration
    if (*accy > maxAcc) *accy = maxAcc; 
    if (*accy < maxAcc - 128) *accy = maxAcc - 128;
}

int main() {
    // Make exiting with the home button possible
    setup_callbacks();

    SceCtrlData pad;
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

    pspDebugScreenInit();

    // spawn player at the center of the screen
    Triangle player = {240.f, 136.f, 20.f, 34.f};
    Asteroid rock = {200, 100, 40, 40};
    
    initBullet();

    // Setup the library used for rendering
    initGu();

    // default, not moving
    float accx = 128.f;
    float accy = 128.f;
    float velx = 0, vely = 0;

    short int pewTimer = 0;
    char setDebug = 1;

    running = 1;
    while(running){
        startFrame();
        
        pspDebugScreenSetXY(0, 2);
        sceCtrlReadBufferPositive(&pad, 1); 

        accx -= (pad.Lx  - 128) / 50;
        accy -= (pad.Ly  - 128) / 50;

        velx = (accx - pad.Lx) / 80; // immitate acceleration and no gravity  
        vely = (accy - pad.Ly) / 80;                                          
                
        player.x -= velx; 
        player.y -= vely; 

        handleSpeed(&accx, &accy);
        
        handleArea(&player.x, &player.y);
        
        updateBullets();

        if (pad.Buttons != 0)
        {
            if (pad.Buttons & PSP_CTRL_LTRIGGER)
            {
                player.angle -= 0.06f;
            }
            if (pad.Buttons & PSP_CTRL_RTRIGGER)
            {
                player.angle += 0.06f;
            }

            //make rotation a little nicer
            if (player.angle < 0) player.angle += 2 * M_PI;
            if (player.angle >= 2 * M_PI) player.angle -= 2 * M_PI;

            if (pad.Buttons & PSP_CTRL_CROSS)
            {
                if (!pewTimer)
                {
                    for (int i = 0; i < MAX_BULLETS; i++)
                    {
                        if (!pew[i].active)
                        {
                            float peakx, peaky;
                            getTriPeak(&player, &peakx, &peaky);

                            // Spawn a new bullet
                            pew[i].x = peakx;
                            pew[i].y = peaky;
                            pew[i].angle = player.angle + (90.f * M_PI / 180.f);
                            pew[i].speed = 8.0f; // Set bullet speed
                            pew[i].active = 1;   // Mark as active
                            pewTimer = 15;
                            break; 
                        }
                    } 
                }
            }
        }      

        if (pewTimer > 0)
        {
            pewTimer--;
        }
        
        rock.angle+= 0.01; 

        printf("Analog X = %3d, ", pad.Lx);
        printf("Analog Y = %3d \n", pad.Ly);
        
        printf("player x = %.4f\n", player.x);
        printf("player y = %.4f\n", player.y);

        printf("acc x = %.1f\n", accx);
        printf("acc y = %.1f\n", accy);
        
        printf("total pews %zuB\n", sizeof(pew));
        printf("total rocks %zuB\n", sizeof(rock));
        
        drawAsteroid(&rock);
        drawTriangle(&player);

        endFrame();
    }

    return 0;
}
