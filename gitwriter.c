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

#include "gitwriter.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

static int rewrite_filename_chars(char* dest, const char* source)
{
    int result;
    for(result=0; *source != '\0'; source++)
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

static void wikiDateToGitDate(char* dest, const char* source)
{
    static const char* month_names[] = {"Jan", "Feb", "Mar", "Apr", "May",
                                   "Jun", "Jul", "Aug", "Sep", "Oct",
                                   "Nov", "Dec"};
    /* Source: <timestamp>2001-05-30T11:37:26Z</timestamp>
                          01234567890123456789
       Dest:   05 May 11:37:26 2001
               01234567890123456789*/
    strncpy(dest, source + 8, 2);
    unsigned month = source[6] - '0';
    if((month < 3) && (source[5] == '1'))
    {
        month += 10;
    }
    assert(month <= 12);
    strncpy(dest+3, month_names[month-1], 3);
    strncpy(dest+7, source+11, 8);
    strncpy(dest+16, source, 4);
}

static void print_author(FILE* out, struct RevData const* revision, const char* date)
{
    if(!stringIsEmpty(&revision->user))
    {
        fprintf(out, "author %s <%s> %s\n", revision->user.data, revision->user.data, date);
    }
}

void commit_rev(FILE* out, struct CommitData const* commit)
{
    static struct DynString file_name;
    static char git_date[] = "22 Feb 05:42:23 2097";
    wikiDateToGitDate(git_date, commit->revision->time.data);
    create_filename(&file_name, commit->title);
    fprintf(out, "commit refs/heads/%s\n", file_name.data);
    print_author(out, commit->revision, git_date);
    fprintf(out, "committer Elevation <elevation@example.com> %s\n", git_date);
    if(!stringIsEmpty(&commit->revision->comment))
    {
        fprintf(out, "data %d\n%s\n", stringLength(&commit->revision->comment), commit->revision->comment.data);
    }
    else
    {
        fprintf(out, "data <<EOF\n/* no comment */\nEOF\n\n");
    }
    if(commit->start_branch)
    {
        fprintf(out, "from refs/heads/meta\n");
    }
    fprintf(out, "M 644 :%d content.mediawiki\n", commit->revision->blobref);
}

void commit_site_info(FILE* out, struct SiteinfoData const* site)
{
    fprintf(out, "commit refs/heads/meta\n");
    fprintf(out, "committer Elevation <elevation@example.com> 15 Jan 12:00:00 2001\n");
    fprintf(out, "data <<EOF\nInitial meta data\nEOF\n\n"
           "M 644 inline meta/siteinfo.txt\n"
           "data <<EOF_65a495a\n"
           "%s\n"
           "%s\n\n"
           "EOF_65a495a\n\n", site->name->data, site->base->data);
}

void start_blob(FILE* out, struct RevData const* revision)
{
    fprintf(out, "blob\nmark :%d\ndata <<EOF_%p_PAGE\n", revision->blobref, revision);
}

void stop_blob(FILE* out, struct RevData const* revision)
{
    fprintf(out, "\nEOF_%p_PAGE\n\n", revision);
}

void progress(FILE* out, const char* what, ...)
{
    va_list params;
    fprintf(out, "progress ");
    va_start(params, what);
    vfprintf(out, what, params);
    va_end(params);
    fprintf(out, "\n");
}
