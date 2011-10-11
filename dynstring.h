#ifndef DYNSTRING_H__
#define DYNSTRING_H__

#include <stdbool.h>

#ifdef __cplusplus
extern "C"{
#endif

struct DynString
{
    char* data;
    int length;
    int size;
};

int appendString(struct DynString* string, const char* input, int length);
bool stringIsEmpty(struct DynString* string);
void clearString(struct DynString* string);
void freeString(struct DynString* string);

#ifdef __cplusplus
}
#endif

#endif
