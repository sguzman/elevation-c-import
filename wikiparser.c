#include "wikiparser.h"
#include <string.h>
#include <stddef.h>

#include "dynstring.h"
#include "gitwriter.h"

#define TAGDEF(name, _a, _b, _act, _aparam) name,
#define TARGET(_name)
#define REV_TARGET(_name)
enum CurrentTag{
#include "tags.inc"
};
#undef REV_TARGET
#undef TARGET
#undef TAGDEF

enum TagAction{actNone, actStore, actCheckStore, actBlob, actCleanSite, actCleanPage, actCleanRev};

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
    struct RevData revision;
};

#define TARGET(name) (offsetof(struct ParserState, name))
#define REV_TARGET(name) (offsetof(struct ParserState, revision.name))
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
        break;

        case actCleanRev:
            clearString(&state->revision.time);
            clearString(&state->revision.comment);
            clearString(&state->revision.ip);
            clearString(&state->revision.user);
        break;

        case actBlob:
            state->revision.blobref++;
            start_blob(&state->revision);
        break;

        case actCheckStore:
            if(!stringIsEmpty(&state->pageTitle))
            {
                fprintf(stderr, "Error: more than one title in page %s", state->pageTitle.data);
                exit(1);
            }
        break;
        default: // ignore other actions
        break;
    }
}

static void wikiHandleStopElement(struct ParserState* state)
{
    switch(tags[state->tag].action)
    {
        case actBlob:
            stop_blob(&state->revision);
        break;

        case actCleanRev:
        {
            struct CommitData const commit = {
                &state->revision,
                &state->pageTitle,
                false,
                "01 Apr 12:23:42 2000",
                "Jon Doe <jon.doe@example.com>"
            };
            commit_rev(&commit);
        }
        break;

        case actCleanSite:
        {
            struct SiteinfoData const site = {
                &state->siteName,
                &state->siteBase,
                "01 Apr 12:23:42 2000",
                "Jon Doe <jon.doe@example.com>"
            };
            commit_site_info(&site);
        }
        break;

        default:
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
}

static void wikiEndElement(void* context, const xmlChar* name)
{
    struct ParserState* state = context;
    if(!strcmp(tags[state->tag].tag, (const char*)name))
    {
        wikiHandleStopElement(state);
        state->tag = tags[state->tag].root;
    }
}

static void wikiGetText(void* context, const xmlChar* content, int len)
{
    struct ParserState* const state = context;
    const enum TagAction action = tags[state->tag].action;
    if((actStore == action) || (actCheckStore == action))
    {
        struct DynString* const string = context + tags[state->tag].actionParameter;
        appendString(string, (const char*)content, len);
    }
    else if(actBlob == action)
    {
        fwrite(content, len, 1, stdout);
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
}

int parseWiki(const char* file)
{
    xmlDefaultSAXHandlerInit();
    xmlSAXHandler handler;
    struct ParserState state;
    initWikiParser(&handler, &state);
    return xmlSAXUserParseFile(&handler, &state, file);
}
