#ifndef GITWRITER_H__
#define GITWRITER_H__

#include <stdbool.h>

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

struct CommitData
{
    struct RevData* revision;
    struct DynString* title;
    const char* start_branch;
    const char* date;
    const char* user;
};

void start_blob(struct RevData const* revision);
void stop_blob(struct RevData const* revision);
void commit_rev(struct CommitData const* details);

#ifdef __cplusplus
}
#endif

#endif
