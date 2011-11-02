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

#define FILE_COUNT ('Z'-'A'+1 /* One entry for each letter */ \
                         + 1  /* One Entry for other chars */ \
                         +1   /* The base-entry */)

struct OutFile
{
    FILE* targets[FILE_COUNT];
    const char* name_template;
    int name_template_length;
    bool make_fifo;

    struct DynString filename_cache;
};

void files_init(struct OutFile* of, char const* name_template, bool make_fifo);

void files_open(struct OutFile* of);

void files_close_meta(struct OutFile* of);

void files_close(struct OutFile* of);

FILE* files_get_page(struct OutFile* of, char const* page_title);

FILE* files_get_meta(struct OutFile* of);

/* private API, exposed for test purposes only. */

char files_page_character(char const* page_title);

int files_convert_char(char in);

#ifdef __cplusplus
}
#endif

#endif
