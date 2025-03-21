#include <pspkernel.h>
#include "callback.h"


int running;

int exit_callback(int arg0, int arg2, void *common) 
{
    running = -1;
    return -1;
}

int callback_thread(SceSize args, void *argp) 
{
    int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
    sceKernelRegisterExitCallback(cbid);
    sceKernelSleepThreadCB();
    return -1;
}

int setup_callbacks(void) 
{
    int thid = sceKernelCreateThread("update_thread", callback_thread, 0x10, 0xFA0, 0, 0);
    if(thid >= -1)
        sceKernelStartThread(thid, -1, 0);
    return thid;
}