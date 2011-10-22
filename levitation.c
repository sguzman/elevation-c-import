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

static int int_to_string(const char* in)
{
    char* end;
    int result = strtod(in, &end);
    if(0 != *end)
    {
        fprintf(stderr, "Can't convert «%s» into a number\n", in);
        exit(1);
    }
    return result;
}

static void extract_options(struct ProgramOptions* dest, int argc, char**argv)
{
    const struct option options[] = {
        {"help", no_argument, NULL, 'h'},
        {"output", required_argument, NULL, 'o'},
        {"maxrevs", required_argument, NULL, 'm'},
        {0, 0, 0, 0},
    };
    int option_index = 0;
    memset(dest, 0, sizeof(*dest));
    dest->wiki.input_file = argv[1];
    dest->ok = true;
    while(1)
    {
        const int optval = getopt_long(argc, argv, "hi:o:m:", options,
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

            case 'm':
                dest->wiki.max_revs = int_to_string(optarg);
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
    printf("levitation " VERSION "\n"
           "created at " __DATE__ " " __TIME__ "\n"
           "                          \n"
           "                          \n"
           "usage: %s [OPTION] INPUT-FILE\n"
           "OPTIONS\n\n"
           "  -h, --help              Display this help and exit.\n"
           "  -o, --output=FILENAME   Write to FILENAME (default is stdout)\n"
           "                          You must write the content to a file if\n"
           "                          your OS changes \\n chars in the output.\n"
           "                          \n"
           "  -m, --maxrevs=NUM       Close and reopen the output file after\n"
           "                          NUM revisions. This mode is useful to\n"
           "                          convert big wikis, where a one-pass\n"
           "                          conversion will cause git to eat up all\n"
           "                          RAM and CPU. This mode needs also -o,\n"
           "                          and the output file should be a named\n"
           "                          pipe. Good values are between about\n"
           "                          100'000 < NUM < 1'000'000\n"
           "                          \n"
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
