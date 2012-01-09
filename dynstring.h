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

#ifndef DYNSTRING_H__
#define DYNSTRING_H__

#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C"{
#endif

/** \brief Dynamically growing string.
 *
 * This structure holds the technical details of a string.*/
struct DynString
{
    /** This pointer holds the string data. */
    char* data;

    /** The current length of the string data */
    int length;

    /** The size of the data block allocated in \sa data */
    int size;

    /** The total number of allocations done. */
    int allocations;

    /** The length of the larges string stored in this string. Used for
      * statistics only.*/
    int max_length;
};

/** \brief append \p length chars from \p input to \p string. */
void appendString(struct DynString* string, const char* input, int length);

/** \brief return true when the \p string contains no chars, false otherwise. */
bool stringIsEmpty(struct DynString const* string);

/** \brief returns the length of \p string. */
int stringLength(struct DynString const* string);

/** \brief ensures that \p string can store at least \p new_size chars. */
void setStringMinCapacity(struct DynString* string, int new_size);

/** \brief clears the \p string.
  *
  * The string will be set into a state such that a call to \sa stringIsEmpty()
  * afterwards will return true. The memory allocated to this string will still
  * be allocated, there is no call to free() by this function. */
void clearString(struct DynString* string);

/** \brief Destroys the string.
  *
  * This function returns the memory allocated by \p string to the OS. */
void freeString(struct DynString* string);

/** \brief Print some memory statistics.
  *
  * This function prints the allocation count, string size and used size of
  * \p string in form of a git progress statement to \p out. To differentiate
  * the statistics about different strings, \p label is used to generate an
  * unique output for each string. */
void printStatistic(struct DynString const* string, const char* label, FILE* out);

#ifdef __cplusplus
}
#endif

#endif
