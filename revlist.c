#include "revlist.h"

#include <stdlib.h>
#include <stdio.h>

static void mark_invalid(struct RevisionList* list)
{
    clearString(&list->revision.time);
    clearString(&list->revision.comment);
    clearString(&list->revision.ip);
    clearString(&list->revision.user);
}

static struct RevisionList* alloc_revlist(void)
{
    struct RevisionList* result = calloc(1, sizeof(*result));
    if(!result)
    {
        fprintf(stderr, "Can't allocate a new revision\n");
        exit(1);
    }
    mark_invalid(result);
    return result;
}

void revision_init(struct RevisionStore* store)
{
    store->base = store->current = alloc_revlist();
}

void revision_new(struct RevisionStore* store)
{
    if(store->current->next)
    {
        store->current = store->current->next;
        mark_invalid(store->current);
    }
    else
    {
        struct RevisionList* new_rev = alloc_revlist();
        store->current->next = new_rev;
        store->current = new_rev;
    }
}

void revision_release(struct RevisionStore* store)
{
    store->current = store->base;
}
