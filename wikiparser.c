#include "wikiparser.h"

static void levStartElement(void* context, const xmlChar* name, const xmlChar** attribs)
{
    printf("+ %s\n", name);
}

void initWikiParser(xmlSAXHandler* target)
{
    memset(target, 0, sizeof(*target));
    target->startElement = levStartElement;
}

