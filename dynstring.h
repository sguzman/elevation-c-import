#ifndef DYNSTRING_H__
#define DYNSTRING_H__

#ifdef __cplusplus
extern "C"{
#endif

struct DynString
{
    char* data;
    int length;
    int size;
};

int setString(struct DynString* string, char* input);
int clearString(struct DynString* string);

#ifdef __cplusplus
}
#endif

#endif
