#include "dynstring.h"

#include <string.h>
#include <stdlib.h>

int appendString(struct DynString* string, const char* input, int length)
{
    const int totalSize = string->length + length + 1;
    if(totalSize > string->size)
    {
        string->size= 0;
        string->data = realloc(string->data, totalSize);
        if(!string->data)
        {
            return -1;
        }
        string->size = totalSize;
    }
    strncpy(string->data + string->length, input, length);
    string->length = totalSize - 1;
    string->data[totalSize - 1] = 0;
    return 0;
}

void clearString(struct DynString* string)
{
    string->length = 0;
}

void freeString(struct DynString* string)
{
    free(string->data);
}
