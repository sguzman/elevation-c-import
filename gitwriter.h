#ifndef GITWRITER_H__
#define GITWRITER_H__

#include "dynstring.h"

#ifdef __cplusplus
extern "C"{
#endif

struct RevData
{
    struct DynString time;
    struct DynString comment;
    struct DynString ip;
    struct DynString user;
    int blobref;
};

#ifdef __cplusplus
}
#endif

#endif
