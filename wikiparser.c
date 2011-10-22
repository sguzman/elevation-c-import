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
#include <time.h>
#include <math.h>


#include "dynstring.h"
#include "gitwriter.h"
#include "version.h"
#include "filehandling.h"

#define TAGDEF(name, _a, _b, _act, _aparam) name,
#define TARGET(_name)
#define REV_TARGET(_name)
enum CurrentTag{
#include "tags.inc"
};
#undef REV_TARGET
#undef TARGET
#undef TAGDEF

enum TagAction{actNone, actStore, actCheckStore, actBlob, actCleanSite,
    actCleanPage, actCleanRev, actExit};

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
    /** Total number of revisions. */
    int  page_revisions;
    /** Maximum number of revisions which should go in one file. */
    int close_revisions;
    /** Number of revisions int he current file. */
    int current_revisions;
    struct OutFile file;
    time_t convert_start;
    time_t page_start;
};

#define TARGET(name) (offsetof(struct ParserState, name))
#define REV_TARGET(name) (offsetof(struct ParserState, revision.name))
#define TAGDEF(sym, tag, root, action, param) {tag, #sym, root, action, param},
static struct TagInfo const tags[] ={
#include "tags.inc"
};
#undef TAGDEF

#define ARRAY_SIZE(array) (sizeof(array)/sizeof(*array))

static void reopen_output(struct ParserState* state)
{
    outfile_advance(&state->file);
    progress(state->file.out, "Reopened output");
    state->current_revisions = 0;
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
            state->page_revisions = 0;
            state->page_start = time(NULL);
            if(state->close_revisions &&
               (state->current_revisions >= state->close_revisions))
            {
                reopen_output(state);
            }
        break;

        case actCleanRev:
            clearString(&state->revision.time);
            clearString(&state->revision.comment);
            clearString(&state->revision.ip);
            clearString(&state->revision.user);
        break;

        case actBlob:
            state->revision.blobref++;
            start_blob(state->file.out, &state->revision);
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

static double rev_per_sec(double revisions, time_t start, time_t stop)
{
    const unsigned delta = stop - start;
    if(delta)
    {
        return revisions/delta;
    }
    return INFINITY;
}

static void wikiHandleStopElement(struct ParserState* state)
{
    switch(tags[state->tag].action)
    {
        case actBlob:
            stop_blob(state->file.out, &state->revision);
        break;

        case actCleanRev:
        {
            struct CommitData const commit = {
                &state->revision,
                &state->pageTitle,
                0 == state->page_revisions,
            };
            commit_rev(state->file.out, &commit);
            state->page_revisions++;
            state->current_revisions++;
        }
        break;

        case actCleanSite:
        {
            struct SiteinfoData const site = {
                &state->siteName,
                &state->siteBase,
            };
            commit_site_info(state->file.out, &site);
            if(state->close_revisions)
            {
                reopen_output(state);
            }
        }
        break;

        case actCleanPage:
        {
            time_t const now = time(NULL);
            fprintf(state->file.out, "progress Overall revisions: %10d (%2.1f rps), "
                                "Imported %5d revisions (%2.1f rps) for %s\n",
                   state->revision.blobref,
                   rev_per_sec(state->revision.blobref, state->convert_start, now),
                   state->page_revisions,
                   rev_per_sec(state->page_revisions, state->page_start, now),
                   state->pageTitle.data);
        }
        break;

        case actExit:
        {
            /* print statistics and clean up*/
            time_t const now = time(NULL);
            fprintf(state->file.out, "progress Wrote %d pages in %ld sec (%2.1f rps)\n",
                    state->revision.blobref,
                    now - state->convert_start,
                    rev_per_sec(state->revision.blobref, state->convert_start, now));
#define PRINT_STAT(x) printStatistic(&state->x, #x, state->file.out)
            PRINT_STAT(siteName);
            PRINT_STAT(siteBase);
            PRINT_STAT(pageTitle);
            PRINT_STAT(revision.time);
            PRINT_STAT(revision.comment);
            PRINT_STAT(revision.ip);
            PRINT_STAT(revision.user);
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
        fwrite(content, len, 1, state->file.out);
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
    state->convert_start = time(NULL);
    state->file.name_template = wpi->output_name;
    state->file.make_fifo = wpi->make_fifo;
    state->close_revisions = wpi->max_revs;
    reopen_output(state);
}

int parseWiki(struct WikiParserInfo const* wpi)
{
    xmlDefaultSAXHandlerInit();
    xmlSAXHandler handler;
    struct ParserState state;
    initWikiParser(&handler, &state, wpi);
    progress(state.file.out, "levitation version " VERSION "(" __DATE__ " " __TIME__ ")");
    return xmlSAXUserParseFile(&handler, &state, wpi->input_file);
}
