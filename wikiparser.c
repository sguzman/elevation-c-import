#include "wikiparser.h"
#include <string.h>
#include <stddef.h>
#include <stdlib.h>

#include "dynstring.h"
#include "revlist.h"

#define TAGDEF(name, _a, _b, _act, _aparam) name,
#define TARGET(_name)
#define REV_TARGET(_name)
enum CurrentTag{
#include "tags.inc"
};
#undef REV_TARGET
#undef TARGET
#undef TAGDEF

enum TagAction{actNone, actStore, actRevStore, actCheckStore, actBlob, actCleanSite, actCleanPage, actInitRev};

struct TagInfo
{
    const char* tag;
    const char* c_name;
    enum CurrentTag root;
    enum TagAction action;
    int actionParameter;
};

struct ParserState
{
    enum CurrentTag tag;
    struct DynString siteName;
    struct DynString siteBase;
    struct DynString pageTitle;
    struct RevisionStore revs;
    int blobref;
};

#define TARGET(name) (offsetof(struct ParserState, name))
#define REV_TARGET(name) (offsetof(struct RevisionList, revision.name))
#define TAGDEF(sym, tag, root, action, param) {tag, #sym, root, action, param},
static struct TagInfo const tags[] ={
#include "tags.inc"
};
#undef TAGDEF

#define ARRAY_SIZE(array) (sizeof(array)/sizeof(*array))

static void wikiHandleStartElement(struct ParserState* state)
{
    switch(tags[state->tag].action)
    {
        case actCleanSite:
            clearString(&state->siteName);
            clearString(&state->siteBase);
        break;

        case actCleanPage:
            clearString(&state->pageTitle);
            revision_release(&state->revs);
        break;

        case actInitRev:
            revision_new(&state->revs);
        break;

        case actCheckStore:
            if (!stringIsEmpty(&state->pageTitle))
            {
                fprintf(stderr, "mehrere Seitentitel für %s\n", state->pageTitle.data);
                exit(1);
            }
        break;

        default: // ignore other actions
        break;
    }
}

static void wikiStartElement(void* context, const xmlChar* name, const xmlChar** attribs)
{
    struct ParserState* state = context;
    int i;
    for(i = 0; i < ARRAY_SIZE(tags); ++i)
    {
        if(tags[i].root == state->tag)
        {
            if(!strcmp(tags[i].tag, (const char*)name))
            {
                state->tag = i;
                wikiHandleStartElement(state);
                return;
            }
        }
    }
    printf("Ignore tag %s in state %s\n", name, tags[state->tag].c_name);
}

static void wikiEndElement(void* context, const xmlChar* name)
{
    struct ParserState* state = context;
    if(!strcmp(tags[state->tag].tag, (const char*)name))
    {
        //printf("up      %s->%s\n", tags[state->tag].c_name, tags[tags[state->tag].root].c_name);
        state->tag = tags[state->tag].root;
    }
}

static void wikiGetText(void* context, const xmlChar* content, int len)
{
    struct ParserState* state = context;
    struct DynString* string = NULL;

    if(actStore == tags[state->tag].action)
    {
        string = context + tags[state->tag].actionParameter;
    }

    if(actRevStore == tags[state->tag].action)
    {
        string = ((void*)state->revs.current) + tags[state->tag].actionParameter;
    }

    if(string)
    {
        appendString(string, (const char*)content, len);
        printf("Store %s->%s\n", tags[state->tag].c_name, string->data);
    }
}

void initWikiParser(xmlSAXHandler* target, struct ParserState* state)
{
    memset(target, 0, sizeof(*target));
    target->startElement = wikiStartElement;
    target->endElement = wikiEndElement;
    target->characters = wikiGetText;

    memset(state, 0, sizeof(*state));
    state->tag = ctNone;
    revision_init(&state->revs);
}

int parseWiki(const char* file)
{
    xmlDefaultSAXHandlerInit();
    xmlSAXHandler handler;
    struct ParserState state;
    initWikiParser(&handler, &state);
    return xmlSAXUserParseFile(&handler, &state, file);
}
