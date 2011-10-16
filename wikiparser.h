#ifndef WIKIPARSER_H__
#define WIKIPARSER_H__

#include <stdio.h>

#ifdef __cplusplus
extern "C"{
#endif

int parseWiki(const char* file, FILE* output, const char* committer,
              const char* date);

#ifdef __cplusplus
}
#endif

#endif
