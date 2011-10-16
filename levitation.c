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

static void extract_options(struct ProgramOptions* dest, int argc, char**argv)
{
    const struct option options[] = {
        {"help", no_argument, NULL, 'h'},
        {"output", required_argument, NULL, 'o'},
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

            case 'n':
                dest->wiki.committer = optarg;
            break;

            case 'd':
                dest->wiki.date = optarg;
            break;
        }
    }
    if(optind == (argc -1))
    {
        dest->wiki.input_file = argv[optind];
    }
    dest->ok = dest->wiki.input_file || dest->help;
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
    parseWiki(&opts.wiki);
    return 0;
}
