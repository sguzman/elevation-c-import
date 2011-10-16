#ifndef GITWRITER_H__
#define GITWRITER_H__

#include <stdbool.h>

#include "dynstring.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef int mark_type;

struct RevData
{
    struct DynString time;
    struct DynString comment;
    struct DynString ip;
    struct DynString user;
    mark_type blobref;
};

struct CommitData
{
    struct RevData const* revision;
    struct DynString const* title;
    bool start_branch;
    const char* date;
    const char* user;
};

struct SiteinfoData
{
    struct DynString const* name;
    struct DynString const* base;
    const char* date;
    const char* user;
    mark_type* blobref;
};

void start_blob(struct RevData const* revision);
void stop_blob(struct RevData const* revision);
void commit_rev(struct CommitData const* details);
void commit_site_info(struct SiteinfoData const* site);

#ifdef __cplusplus
}
#endif

#endif
