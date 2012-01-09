/*  This is part of elevation, the  MediaWiki to git trasformator.
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

/** \brief Own data type for the mark commands in the output stream. */
typedef int mark_type;

/** \brief Data about one single revision of a page. */
struct RevData
{
    struct DynString time;
    struct DynString comment;
    struct DynString ip;
    struct DynString user;
    mark_type blobref;
};

/** \brief The data of one git commit. */
struct CommitData
{
    struct RevData const* revision;
    struct DynString const* title;
    bool start_branch;
};

/** \brief The Data of the meta data of the wiki. */
struct SiteinfoData
{
    struct DynString const* name;
    struct DynString const* base;
    mark_type* blobref;
};

/** \brief Emmit a BLOB command to the output file. */
void start_blob(FILE* out, struct RevData const* revision);

/** \brief Emmit the end of the BLOB command to the output file. */
void stop_blob(FILE* out, struct RevData const* revision);

/** \brief Emmit a COMMIT command to the output file. */
void commit_rev(FILE* out, struct CommitData const* details);

/** \brief Create the `meta` commit. */
void commit_site_info(FILE* out, struct SiteinfoData const* site);

/** \brief Put a PROGRESS command to the output file.
  *
  * `git fast-import` later displays this message. */
void progress(FILE* out, const char* what, ...) __attribute__ ((format (printf, 2, 3)));


#ifdef __cplusplus
}
#endif

#endif
