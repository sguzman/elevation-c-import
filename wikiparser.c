#include "wikiparser.h"
#include <string.h>
#include <stddef.h>

#include "dynstring.h"

#define TAGDEF(name, _a, _b, _act, _aparam) name,
#define TARGET(_name)
#define REV_TARGET(_name)
enum CurrentTag{
#include "tags.inc"
};
#undef REV_TARGET
#undef TARGET
#undef TAGDEF

enum TagAction{actNone, actStore, actCheckStore, actBlob, actCleanSite, actCleanPage, actCleanRev};

struct TagInfo
{
    const char* tag;
    const char* c_name;
    enum CurrentTag root;
    enum TagAction action;
    int actionParameter;
};

struct RevData
{
    struct DynString time;
    struct DynString comment;
    struct DynString ip;
    struct DynString user;
    int blobref;
};

struct ParserState
{
    enum CurrentTag tag;
    struct DynString siteName;
    struct DynString siteBase;
    struct DynString pageTitle;
    struct RevData revision;
};

#define TARGET(name) (offsetof(struct ParserState, name))
#define REV_TARGET(name) (offsetof(struct ParserState, revision.name))
#define TAGDEF(sym, tag, root, action, param) {tag, #sym, root, action, param},
static struct TagInfo const tags[] ={
#include "tags.inc"
};
#undef TAGDEF

#define ARRAY_SIZE(array) (sizeof(array)/sizeof(*array))

static int rewrite_filename_chars(char* dest, const char* source)
{
    int result;
    for(result=0; *source != '\0'; result++, source++)
    {
        const char input = *source;
        if(    ((input >= '0')&& (input <= '9'))
            || ((input >= 'a')&& (input <= 'z'))
            || ((input >= 'A')&& (input <= 'Z'))
            || (input == '_') || (input == '-')
            || (input == '+'))
        {
            if(dest)
            {
                dest[result] = *source;
            }
            result++;
        }
        else if(input == ' ')
        {
            if(dest)
            {
                dest[result] = '_';
            }
            result++;
        }
        else
        {
            if(dest)
            {
                sprintf(dest+result, "%%%02x", (unsigned char)input);
            }
            result += 3;
        }
    }
    if(dest)
    {
        dest[result] = '\0';
    }
    result++;
    return result;
}

static void create_filename(struct DynString* dest, struct DynString const* source)
{
    setStringMinCapacity(dest, rewrite_filename_chars(NULL, source->data));
    rewrite_filename_chars(dest->data, source->data);
}

static void wikiDateToGitDate(struct DynString* dest, struct DynString const* source)
{
}

static void print_author(struct RevData const* revision, struct DynString const* date)
{
    if(!stringIsEmpty(&revision->user))
    {
        printf("author %s <%s> %s\n", revision->user.data, revision->user.data, date->data);
    }
}

static void commit_rev(struct RevData const* revision, struct DynString const* title)
{
    static struct DynString file_name;
    static struct DynString git_date;
    wikiDateToGitDate(&git_date, &revision->time);
    create_filename(&file_name, title);
    printf("commit refs/heads/%s\n", file_name.data);
    print_author(revision, &git_date);
}

static void start_blob(struct RevData const* revision)
{
    printf("blob\nmark :%d\ndata <<EOF_%p_PAGE\n", revision->blobref, revision);
}

static void stop_blob(struct RevData const* revision)
{
    printf("\nEOF_%p_PAGE\n\n", revision);
}

static void wikiHandleStartElement(struct ParserState* state)
{
    switch(tags[state->tag].action)
    {
        case actCleanSite:
            clearString(&state->siteName);
            clearString(&state->siteBase);
        break;

        case actCleanPage:
            clearString(&state->pageTitle);
        break;

        case actCleanRev:
            clearString(&state->revision.time);
            clearString(&state->revision.comment);
            clearString(&state->revision.ip);
            clearString(&state->revision.user);
        break;

        case actBlob:
            state->revision.blobref++;
            start_blob(&state->revision);
        break;

        case actCheckStore:
            if(!stringIsEmpty(&state->pageTitle))
            {
                fprintf(stderr, "Error: more than one title in page %s", state->pageTitle.data);
                exit(1);
            }
        break;
        default: // ignore other actions
        break;
    }
}

static void wikiHandleStopElement(struct ParserState* state)
{
    switch(tags[state->tag].action)
    {
        case actBlob:
            stop_blob(&state->revision);
        break;

        case actCleanRev:
            commit_rev(&state->revision, &state->pageTitle);
        break;

        default:
        break;
    }
}

static void wikiStartElement(void* context, const xmlChar* name, const xmlChar** attribs)
{
    struct ParserState* state = context;
    int i;
    for(i = 0; i < ARRAY_SIZE(tags); ++i)
    {
        if(tags[i].root == state->tag)
        {
            if(!strcmp(tags[i].tag, (const char*)name))
            {
                state->tag = i;
                wikiHandleStartElement(state);
                return;
            }
        }
    }
}

static void wikiEndElement(void* context, const xmlChar* name)
{
    struct ParserState* state = context;
    if(!strcmp(tags[state->tag].tag, (const char*)name))
    {
        wikiHandleStopElement(state);
        state->tag = tags[state->tag].root;
    }
}

static void wikiGetText(void* context, const xmlChar* content, int len)
{
    struct ParserState* const state = context;
    const enum TagAction action = tags[state->tag].action;
    if((actStore == action) || (actCheckStore == action))
    {
        struct DynString* const string = context + tags[state->tag].actionParameter;
        appendString(string, (const char*)content, len);
        printf("progress %s\n", string->data);
    }
    else if(actBlob == action)
    {
        fwrite(content, len, 1, stdout);
    }
}

void initWikiParser(xmlSAXHandler* target, struct ParserState* state)
{
    memset(target, 0, sizeof(*target));
    target->startElement = wikiStartElement;
    target->endElement = wikiEndElement;
    target->characters = wikiGetText;

    memset(state, 0, sizeof(*state));
    state->tag = ctNone;
}

int parseWiki(const char* file)
{
    xmlDefaultSAXHandlerInit();
    xmlSAXHandler handler;
    struct ParserState state;
    initWikiParser(&handler, &state);
    return xmlSAXUserParseFile(&handler, &state, file);
}
