#include <libxml/SAX2.h>
#include <string.h>
#include <stdio.h>

static void levStartElement(void* context, const xmlChar* name, const xmlChar** attribs)
{
    printf("+ %s\n", name);
}

static void initParser(xmlSAXHandler* target)
{
    memset(target, 0, sizeof(*target));
    target->startElement = levStartElement;
}

int main(int argc, char**argv)
{
    if(argc != 2)
    {
        fprintf(stderr, "Usage: %s filename\n", argv[0]);
        return 1;
    }
    xmlDefaultSAXHandlerInit();
    xmlSAXHandler handler;
    initParser(&handler);
    const int result = xmlSAXUserParseFile(&handler, NULL, argv[1]);
    return 0;
}
