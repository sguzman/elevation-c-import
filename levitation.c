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
    parseWiki(argv[1], stdout, "Jon Doe <jd@example.com>",
              "01 Apr 12:23:42 2000");
    return 0;
}
