#include <string.h>
#include <stdio.h>

#include <libxml/parser.h>

#include "wikiparser.h"

int main(int argc, char**argv)
{
    if(argc != 2)
    {
        fprintf(stderr, "Usage: %s filename\n", argv[0]);
        return 1;
    }
    xmlDefaultSAXHandlerInit();
    xmlSAXHandler handler;
    initWikiParser(&handler);
    const int result = xmlSAXUserParseFile(&handler, NULL, argv[1]);
    return 0;
}
