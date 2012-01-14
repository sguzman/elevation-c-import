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

#include <string.h>
#include <stdio.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdlib.h>

#include "wikiparser.h"
#include "version.h"
#include "gitwriter.h"

struct ProgramOptions
{
    bool help;
    bool ok;
    struct WikiParserInfo wiki;
};

static void extract_options(struct ProgramOptions* dest, int argc, char**argv)
{
    const struct option options[] = {
        {"help", no_argument, NULL, 'h'},
        {"output", required_argument, NULL, 'o'},
        {"maxrevs", required_argument, NULL, 'm'},
#ifndef _WIN32
        {"pipes", no_argument, NULL, 'p'},
#endif
        {0, 0, 0, 0},
    };
    int option_index = 0;
    memset(dest, 0, sizeof(*dest));
    dest->wiki.input_file = argv[1];
    dest->ok = true;
    while(1)
    {
        const int optval = getopt_long(argc, argv, "ho:p", options,
                                       &option_index);
        if(optval == -1)
        {
            break;
        }

        switch(optval)
        {
            case 'h':
                dest->help = true;
            break;

            case 'o':
                dest->wiki.output_name = optarg;
            break;

            case 'p':
                dest->wiki.make_fifo = true;
            break;

            case '?':
                /* getopt already printed an error message */
            break;

            default:
                dest->ok = false;
        }
    }
    if(optind == (argc -1))
    {
        dest->wiki.input_file = argv[optind];
    }
    dest->ok &= dest->wiki.input_file || dest->help;
}

static void display_help(char const* myself)
{
    printf("elevation " VERSION "\n"
           "created at " __DATE__ " " __TIME__ "\n"
           "                          \n"
           "usage: %s [OPTION] INPUT-FILE\n"
           "OPTIONS\n\n"
           "  -h, --help              Display this help and exit.\n"
           "  -o, --output=FILENAME   Write to FILENAME (default is stdout)\n"
           "                          You must write the content to a file if\n"
           "                          your OS changes \\n chars in the output.\n"
           "                          The filename must contain exact one\n"
           "                          `%%c' sequence, which get replaced by the\n"
           "                          first char of each page.\n"
           "                          \n"
#ifndef _WIN32
           " -p, --pipes              Create named pipes instead of files.\n"
           "                          \n"
#endif
           "\n"
           "The INPUT-FILE must be the last parameter on the command line, and\n"
           "is a required parameter.\n",
           myself);
}

int main(int argc, char**argv)
{
    struct ProgramOptions opts;
    extract_options(&opts, argc, argv);
    if(opts.help || !opts.ok)
    {
        display_help(argv[0]);
        return !opts.ok;
    }
    parseWiki(&opts.wiki);
    return 0;
}
