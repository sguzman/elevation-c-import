#include "dynstring.h"

#include <string.h>
#include <stdlib.h>

int setString(struct DynString* string, char* input)
{
    const int inlen = strlen(input);
    if(inlen > string->size)
    {
        free(string->data);
        string->size = 0;
        /* no realloc here, since the memory will be overwritten entirely, so
           the memmove of realloc is not needed */
        string->data = malloc(inlen+1);
        if(!string->data)
        {
            return -1;
        }
        string->size = inlen + 1;
    }
    strcpy(string->data, input);
    string->length = inlen;
    return 0;
}

int clearString(struct DynString* string)
{
    string->length = 0;
}
