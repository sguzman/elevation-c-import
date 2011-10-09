#include "wikiparser.h"
#include <string.h>

#define TAGDEF(name, _a, _b, _act, _aparam) name,
enum CurrentTag{
#include "tags.inc"
};
#undef TAGDEF

enum TagAction{actNone, actStore, actCheckStore, actBlob};

struct TagInfo
{
    const char* tag;
    const char* c_name;
    enum CurrentTag root;
    enum TagAction action;
    int actionParameer;
};

#define TAGDEF(sym, tag, root, action, param) {tag, #sym, root, action, param},
static struct TagInfo const tags[] ={
#include "tags.inc"
};
#undef TAGDEF

#define ARRAY_SIZE(array) (sizeof(array)/sizeof(*array))

struct ParserState
{
    enum CurrentTag tag;
};

static void wikiStartElement(void* context, const xmlChar* name, const xmlChar** attribs)
{
    struct ParserState* state = context;
    int i;
    for(i = 0; i < ARRAY_SIZE(tags); ++i)
    {
        if(tags[i].root == state->tag)
        {
            if(!strcmp(tags[i].tag, BAD_CAST name))
            {
                //printf("transit %s->%s\n", tags[state->tag].c_name, tags[i].c_name);
                state->tag = i;
                return;
            }
        }
    }
    printf("Ignore tag %s in state %s\n", name, tags[state->tag].c_name);
}

static void wikiEndElement(void* context, const xmlChar* name)
{
    struct ParserState* state = context;
    if(!strcmp(tags[state->tag].tag, BAD_CAST name))
    {
        //printf("up      %s->%s\n", tags[state->tag].c_name, tags[tags[state->tag].root].c_name);
        state->tag = tags[state->tag].root;
    }
}

void initWikiParser(xmlSAXHandler* target, struct ParserState* state)
{
    memset(target, 0, sizeof(*target));
    target->startElement = wikiStartElement;
    target->endElement = wikiEndElement;

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
