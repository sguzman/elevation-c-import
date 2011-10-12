#include "dynstring.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void appendString(struct DynString* string, const char* input, int length)
{
    const int totalSize = string->length + length + 1;
    setStringMinCapacity(string, totalSize);
    strncpy(string->data + string->length, input, length);
    string->length = totalSize - 1;
    string->data[totalSize - 1] = 0;
}

bool stringIsEmpty(struct DynString const* string)
{
    return 0 == string->length;
}

int stringLength(struct DynString* string)
{
    return string->length;
}

void setStringMinCapacity(struct DynString* string, int size)
{
    if(size > string->size)
    {
        string->data = realloc(string->data, size);
        if(!string->data)
        {
            fprintf(stderr, "failed to grow %d bytre storage to %d bytes.\n", string->size, size);
            exit(1);
        }
        string->size = size;
    }
}

void clearString(struct DynString* string)
{
    string->length = 0;
}

void freeString(struct DynString* string)
{
    free(string->data);
}
