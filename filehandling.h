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

#ifndef FILEHANDLING_H__
#define FILEHANDLING_H__

#include <stdio.h>
#include <stdbool.h>

#include "dynstring.h"

#ifdef __cplusplus
extern "C"{
#endif

#define FILE_COUNT ('Z'-'A'+1 /* One entry for each letter */ \
                         + 1) /* One Entry for other chars */

/** In which mode should the program run? */
enum OutputMode {omStdout, omOneFile, omManyFiles};

/** \brief This structure holds all FILE pointers for the stream outputs. */
struct OutFile
{
    enum OutputMode output_mode;

    /** \brief The file pointers for the topic (starting with A-Z and special
      * chars) streams */
    FILE* targets[FILE_COUNT];

    /** \brief The file stream for the metadata branch */
    FILE* meta;

    /** \brief The filename template */
    const char* name_template;

    /** \brief The string length of \sa name_template*/
    int name_template_length;

    /** \brief Flag to determine if the output should be realized as named
      * pipes instead of regular files.*/
    bool make_fifo;

    /** \brief A cache to generate the file names for each topic branch. */
    struct DynString filename_cache;
};

/** \brief Opens all topic files. */
void files_init(struct OutFile* of, char const* name_template, bool make_fifo);

/** \brief open the meta file. */
void files_open_meta(struct OutFile* of);

/** \brief Decide if output should go to different files or stdout.
  *
  * If the name template is not NULL, different files will be opened. Else
  * stdout will be assigned to all topic file handles. */
void files_open_dispatch(struct OutFile* of);

/** \brief Closes the meta branch. */
void files_close_meta(struct OutFile* of);

/** \brief closes the topic files. */
void files_close(struct OutFile* of);

/** \brief Returns the topic file handle corresponding to the page title.*/
FILE* files_get_page(struct OutFile* of, char const* page_title);

/** \brief Returns the file handle for the meta branch. */
FILE* files_get_meta(struct OutFile* of);

/* private API, exposed for test purposes only. */

char files_page_character(char const* page_title);

int files_convert_char(char in);

enum TemplateKind{tkNone, tkSingle, tkReplace, tkError};

enum TemplateKind check_template(const char* name_template);

#ifdef __cplusplus
}
#endif

#endif
