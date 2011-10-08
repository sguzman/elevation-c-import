#ifndef WIKIPARSER_H__
#define WIKIPARSER_H__

#ifdef __cplusplus
extern "C"{
#endif

#include <libxml/SAX2.h>

int parseWiki(const char* file);

#ifdef __cplusplus
}
#endif

#endif
