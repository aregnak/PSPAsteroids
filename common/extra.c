#include <stdio.h>
#include <string.h>
#include "extra.h"

void appendIntToBuffer(char* buffer, size_t bufferSize, int num)
{
    size_t len = strlen(buffer);
    if (len < bufferSize - 1) // leave room for null terminator
    {
        snprintf(buffer + len, bufferSize - len, "%d", num);
    }
}
