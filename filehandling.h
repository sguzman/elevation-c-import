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

#ifndef FILEHANDLING_H__
#define FILEHANDLING_H__

#include <stdio.h>
#include <stdbool.h>

#include "dynstring.h"

#ifdef __cplusplus
extern "C"{
#endif

struct OutFile
{
    FILE* out;
    const char* name_template;
    int name_template_length;
    bool make_fifo;

    int current_id;
    struct DynString filename_cache;
};

void outfile_advance(struct OutFile* file);

#ifdef __cplusplus
}
#endif

#endif