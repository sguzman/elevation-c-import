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

#ifndef GITWRITER_H__
#define GITWRITER_H__

#include <stdbool.h>
#include <stdio.h>

#include "dynstring.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef int mark_type;

struct RevData
{
    struct DynString time;
    struct DynString comment;
    struct DynString ip;
    struct DynString user;
    mark_type blobref;
};

struct CommitData
{
    struct RevData const* revision;
    struct DynString const* title;
    bool start_branch;
};

struct SiteinfoData
{
    struct DynString const* name;
    struct DynString const* base;
    mark_type* blobref;
};

void start_blob(FILE* out, struct RevData const* revision);
void stop_blob(FILE* out, struct RevData const* revision);
void commit_rev(FILE* out, struct CommitData const* details);
void commit_site_info(FILE* out, struct SiteinfoData const* site);
void progress(FILE* out, const char* what, ...) __attribute__ ((format (printf, 2, 3)));


#ifdef __cplusplus
}
#endif

#endif
