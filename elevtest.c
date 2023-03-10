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

/* Elevation Testsuite */

#include <assert.h>
#include <stdio.h>

#include "filehandling.h"

void check_fpc(void)
{
    assert( 0  == files_page_character(""));
    assert(':' == files_page_character(":"));
    assert('A' == files_page_character("Ab"));
    assert('a' == files_page_character("ab"));
    assert('A' == files_page_character("foo:A"));
    assert('a' == files_page_character("bar:a"));
    assert('A' == files_page_character(":A"));
    assert('a' == files_page_character(":a"));
    assert('&' == files_page_character("&"));
}

void check_fcc(void)
{
    assert(0 == files_convert_char('a'));
    assert(0 == files_convert_char('A'));
    assert(1 == files_convert_char('b'));
    assert(1 == files_convert_char('B'));
    assert(25 == files_convert_char('z'));
    assert(25 == files_convert_char('Z'));
    assert(26 == files_convert_char(':'));
    assert(26 == files_convert_char('&'));
    assert(26 == files_convert_char('_'));
    assert(26 == files_convert_char('='));
}

void check_ftemplate(void)
{
    /* This function checks the correct work of the file template determining
     * function. */
    assert(tkNone == check_template(NULL));
    assert(tkSingle == check_template(""));
    assert(tkSingle == check_template("abc"));
    assert(tkReplace == check_template("%s"));
    assert(tkReplace == check_template("ab%sc"));
    assert(tkError == check_template("%%"));
    assert(tkError == check_template("%c"));
    assert(tkError == check_template("%s%"));
}

int main(void)
{
    check_fpc();
    check_fcc();
    check_ftemplate();
    printf("OK\n");
    return 0;
}
