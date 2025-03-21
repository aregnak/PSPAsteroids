#ifndef CALLBACK_H
#define CALLBACK_H

#include <pspkernel.h>

extern int running;

int exit_callback(int arg0, int arg2, void *common);
int callback_thread(SceSize args, void *argp); 
int setup_callbacks(void); 

#endif // CALLBACK_H