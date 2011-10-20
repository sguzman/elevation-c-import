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

#include "dynstring.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void appendString(struct DynString* string, const char* input, int length)
{
    const int totalSize = string->length + length + 1;
    setStringMinCapacity(string, totalSize);
    strncpy(string->data + string->length, input, length);
    string->length = totalSize - 1;
    string->data[totalSize - 1] = 0;
}

bool stringIsEmpty(struct DynString const* string)
{
    return 0 == string->length;
}

int stringLength(struct DynString const* string)
{
    return string->length;
}

void setStringMinCapacity(struct DynString* string, int size)
{
    if(size > string->size)
    {
        string->data = realloc(string->data, size);
        if(!string->data)
        {
            fprintf(stderr, "failed to grow %d bytre storage to %d bytes.\n", string->size, size);
            exit(1);
        }
        string->size = size;
        string->allocations++;
        string->max_length = size;
    }
}

void clearString(struct DynString* string)
{
    string->length = 0;
}

void freeString(struct DynString* string)
{
    free(string->data);
}

void printStatistic(struct DynString const* string, const char* label, FILE* out)
{
    fprintf(out, "progress %s: Allocs: %d, size=%d, used=%d\n", label,
            string->allocations, string->size, string->max_length);
}
