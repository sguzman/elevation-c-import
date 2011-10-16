#ifndef WIKIPARSER_H__
#define WIKIPARSER_H__

#include <stdio.h>

#ifdef __cplusplus
extern "C"{
#endif

enum OutputMode{omWriteMeta = 1, omWritePages = 2};

struct WikiParserInfo
{
    char const* input_file;
    char const* committer;
    char const* date;
    FILE* output;
    enum OutputMode mode;
};

int parseWiki(struct WikiParserInfo const* wpi);

#ifdef __cplusplus
}
#endif

#endif
