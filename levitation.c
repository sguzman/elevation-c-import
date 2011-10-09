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
    parseWiki(argv[1]);
    return 0;
}
