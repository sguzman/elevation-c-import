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

void start_blob(struct RevData const* revision);
void stop_blob(struct RevData const* revision);
void commit_rev(struct RevData const* revision, struct DynString const* title, const char* base_rev);

#ifdef __cplusplus
}
#endif

#endif
