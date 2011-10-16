#ifndef WIKIPARSER_H__
#define WIKIPARSER_H__

#include <stdio.h>

#ifdef __cplusplus
extern "C"{
#endif

struct WikiParserInfo
{
    char const* input_file;
    char const* committer;
    char const* date;
    FILE* output;
};

int parseWiki(struct WikiParserInfo const* wpi);

#ifdef __cplusplus
}
#endif

#endif
