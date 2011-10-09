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

#ifdef __cplusplus
}
#endif

#endif
