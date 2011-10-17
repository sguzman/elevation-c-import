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

#include "wikiparser.h"

struct ProgramOptions
{
    bool help;
    bool ok;
    const char* output_filename;
    struct WikiParserInfo wiki;
};

static const char* default_committer = "Jon Doe <jd@example.com>";

struct WriteModeTable
{
    const char* name;
    enum OutputMode mode;
};

static const struct WriteModeTable modes[] =
{
    {"meta", omWriteMeta},
    {"pages", omWritePages},
    {"both", omWritePages | omWriteMeta},
    {NULL, 0},
};

static enum OutputMode parse_mode(const char* mode)
{
    struct WriteModeTable const* mode_inspect = modes;
    for(mode_inspect = modes; mode_inspect->name; mode_inspect++)
    {
        if(!strcmp(mode, mode_inspect->name))
        {
            return mode_inspect->mode;
        }
    }
    return omWriteMeta | omWritePages;
}

static void extract_options(struct ProgramOptions* dest, int argc, char**argv)
{
    const struct option options[] = {
        {"help", no_argument, NULL, 'h'},
        {"output", required_argument, NULL, 'o'},
        {"mode", required_argument, NULL, 'm'},
        {"name", required_argument, NULL, 'n'},
        {"date", required_argument, NULL, 'd'},
        {0, 0, 0, 0},
    };
    int option_index = 0;
    memset(dest, 0, sizeof(*dest));
    dest->wiki.input_file = argv[1];
    dest->wiki.committer = default_committer;
    dest->wiki.date = "01 Apr 12:23:42 2000";
    dest->wiki.output = stdout;
    dest->wiki.mode = omWriteMeta | omWritePages;
    dest->ok = true;
    while(1)
    {
        const int optval = getopt_long(argc, argv, "hi:o:m:n:d:", options,
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
                dest->output_filename = optarg;
            break;

            case 'm':
                dest->wiki.mode = parse_mode(optarg);
            break;

            case 'n':
                dest->wiki.committer = optarg;
            break;

            case 'd':
                dest->wiki.date = optarg;
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
    printf("usage: %s [OPTION] INPUT-FILE\n"
           "OPTIONS\n\n"
           "  -h, --help              Display this help and exit.\n"
           "  -o, --output=FILENAME   Write to FILENAME (default is stdout)\n"
           "                          You must write the content to a file if\n"
           "                          your OS changes \\n chars in the output.\n"
           "                          \n"
           "  -n, --name=NAME         Use NAME as the committer information.\n"
           "                          NAME must be in the format 'name <em@il>'\n"
           "                          Defaults to %s\n"
           "                          \n"
           "  -m, --mode=MODE         Set the output mode to MODE. Valid\n"
           "                          parameters are:\n"
           "                          \n"
           "                          meta:  stop after the wiki meta data\n"
           "                          pages: write only the pages withot meta\n"
           "                          both:  write both meta+pages.\n"
           "                          \n"
           "                          Default is both.\n"
           "                          \n"
           "  -d, --date=DATE         Use DATE as commit date. DATE must be in\n"
           "                          the '01 Apr 12:23:42 2000' format.\n"
           "                          Defaults to now.\n"
           "\n"
           "The INPUT-FILE must be the last parameter on the command line, and\n"
           "is a required parameter.\n",
           myself, default_committer);
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
    if(opts.output_filename)
    {
        opts.wiki.output = fopen(opts.output_filename, "wb");
        if(!opts.wiki.output)
        {
            fprintf(stderr, "Can't write into file `%s'.\n",
                    opts.output_filename);
            return 1;
        }
    }
    parseWiki(&opts.wiki);
    return 0;
}
