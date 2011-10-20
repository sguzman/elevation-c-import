/*  This is part of levitation-2, the  MediaWiki to git trasformator.
    Copyright © 2011  Rüdiger Ranft <_rdi_@web.de>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#include "wikiparser.h"
#include <string.h>
#include <stddef.h>
#include <libxml/SAX2.h>


#include "dynstring.h"
#include "gitwriter.h"

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

struct ParserState
{
    enum CurrentTag tag;
    struct DynString siteName;
    struct DynString siteBase;
    struct DynString pageTitle;
    struct RevData revision;
    int  page_revisions;
    FILE* out;
    enum OutputMode mode;
};

#define TARGET(name) (offsetof(struct ParserState, name))
#define REV_TARGET(name) (offsetof(struct ParserState, revision.name))
#define TAGDEF(sym, tag, root, action, param) {tag, #sym, root, action, param},
static struct TagInfo const tags[] ={
#include "tags.inc"
};
#undef TAGDEF

#define ARRAY_SIZE(array) (sizeof(array)/sizeof(*array))

static void wikiHandleStartElement(struct ParserState* state)
{
    switch(tags[state->tag].action)
    {
        case actCleanSite:
            clearString(&state->siteName);
            clearString(&state->siteBase);
        break;

        case actCleanPage:
            if(!stringIsEmpty(&state->pageTitle))
            {
                fprintf(state->out, "progress Overall revision count: %10d, "
                                    "Imported %5d revisions for %s\n",
                       state->revision.blobref,
                       state->page_revisions, state->pageTitle.data);
            }
            clearString(&state->pageTitle);
            state->page_revisions = 0;
        break;

        case actCleanRev:
            clearString(&state->revision.time);
            clearString(&state->revision.comment);
            clearString(&state->revision.ip);
            clearString(&state->revision.user);
        break;

        case actBlob:
            state->revision.blobref++;
            start_blob(state->out, &state->revision);
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
            stop_blob(state->out, &state->revision);
        break;

        case actCleanRev:
        {
            struct CommitData const commit = {
                &state->revision,
                &state->pageTitle,
                0 == state->page_revisions,
            };
            commit_rev(state->out, &commit);
            state->page_revisions++;
        }
        break;

        case actCleanSite:
        if(state->mode & omWriteMeta)
        {
            struct SiteinfoData const site = {
                &state->siteName,
                &state->siteBase,
            };
            commit_site_info(state->out, &site);
        }
        if(!(state->mode & omWritePages))
        {
            /* Since there are no pages to be written, there is no sense to
             * further process the input XML file.*/
            exit(0);
        }
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
    }
    else if(actBlob == action)
    {
        fwrite(content, len, 1, state->out);
    }
}

void initWikiParser(xmlSAXHandler* target, struct ParserState* state,
                    struct WikiParserInfo const* wpi)
{
    memset(target, 0, sizeof(*target));
    target->startElement = wikiStartElement;
    target->endElement = wikiEndElement;
    target->characters = wikiGetText;

    memset(state, 0, sizeof(*state));
    state->tag = ctNone;
    state->out = wpi->output;
    state->mode = wpi->mode;
}

int parseWiki(struct WikiParserInfo const* wpi)
{
    xmlDefaultSAXHandlerInit();
    xmlSAXHandler handler;
    struct ParserState state;
    initWikiParser(&handler, &state, wpi);
    return xmlSAXUserParseFile(&handler, &state, wpi->input_file);
}
