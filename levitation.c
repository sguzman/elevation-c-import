#include <string.h>
#include <stdio.h>

#include <libxml/parser.h>

#include "wikiparser.h"

struct ProgramOptions
{
    struct WikiParserInfo wiki;
};

static void extract_options(struct ProgramOptions* dest, int argc, char**argv)
{
    memset(dest, 0, sizeof(*dest));
    dest->wiki.input_file = argv[1];
    dest->wiki.committer = "Jon Doe <jd@example.com>";
    dest->wiki.date = "01 Apr 12:23:42 2000";
    dest->wiki.output = stdout;
}

int main(int argc, char**argv)
{
    struct ProgramOptions opts;
    extract_options(&opts, argc, argv);
    parseWiki(&opts.wiki);
    return 0;
}
