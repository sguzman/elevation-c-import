#include "wikiparser.h"
#include <string.h>

#define TAGDEF(name, _a, _b) name,
enum CurrentTag{
#include "tags.inc"
};
#undef TAGDEF

struct TagInfo
{
    const char* tag;
    enum CurrentTag root;
};

#define TAGDEF(_, tag, root) {tag, root},
static struct TagInfo const tags[] ={
#include "tags.inc"
};
#undef TAGDEF

struct ParserState
{
    enum CurrentTag tag;
};

static void wikiStartElement(void* context, const xmlChar* name, const xmlChar** attribs)
{
}

static void wikiEndElement(void* context, const xmlChar* name)
{
}

void initWikiParser(xmlSAXHandler* target)
{
    memset(target, 0, sizeof(*target));
    target->startElement = wikiStartElement;
    target->endElement = wikiEndElement;
}

