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

/* Levitation2 Testsuite */

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

int main(void)
{
    check_fpc();
    printf("OK\n");
    return 0;
}
