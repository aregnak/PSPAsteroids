#ifndef CONFIG_H
#define CONFIG_H

#define BUFFER_WIDTH 512
#define BUFFER_HEIGHT 272

#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 272

// From font example
#define PIXEL_SIZE (4)
#define FRAME_SIZE (BUF_WIDTH * SCR_HEIGHT * PIXEL_SIZE)
#define ZBUF_SIZE (BUF_WIDTH SCR_HEIGHT * 2)

#endif // CONFIG_H