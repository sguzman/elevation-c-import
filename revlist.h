#ifndef REVLIST_H__
#define REVLIST_H__

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

struct RevisionList
{
    struct RevisionList* next;
    struct RevData revision;
};

struct RevisionStore
{
    struct RevisionList* base;
    struct RevisionList* current;
};

/** \brief Initializes the store */
void revision_init(struct RevisionStore* store);

/** \brief Ensures that store->current points to a fresh revision.*/
void revision_new(struct RevisionStore* store);
void revision_release(struct RevisionStore* store);

#ifdef __cplusplus
}
#endif

#endif