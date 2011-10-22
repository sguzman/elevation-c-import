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

#ifndef DYNSTRING_H__
#define DYNSTRING_H__

#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C"{
#endif

struct DynString
{
    char* data;
    int length;
    int size;
    int allocations;
    int max_length;
};

void appendString(struct DynString* string, const char* input, int length);
bool stringIsEmpty(struct DynString const* string);
int stringLength(struct DynString const* string);
void setStringMinCapacity(struct DynString* string, int new_size);
void clearString(struct DynString* string);
void freeString(struct DynString* string);

void printStatistic(struct DynString const* string, const char* label, FILE* out);

#ifdef __cplusplus
}
#endif

#endif
