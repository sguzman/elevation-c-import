#include "gitwriter.h"

#include <stdlib.h>
#include <stdio.h>

static int rewrite_filename_chars(char* dest, const char* source)
{
    int result;
    for(result=0; *source != '\0'; result++, source++)
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

static void wikiDateToGitDate(struct DynString* dest, struct DynString const* source)
{
}

static void print_author(struct RevData const* revision, struct DynString const* date)
{
    if(!stringIsEmpty(&revision->user))
    {
        printf("author %s <%s> %s\n", revision->user.data, revision->user.data, date->data);
    }
}

void commit_rev(struct RevData const* revision, struct DynString const* title, bool start_branch)
{
    static struct DynString file_name;
    static struct DynString git_date;
    wikiDateToGitDate(&git_date, &revision->time);
    create_filename(&file_name, title);
    printf("commit refs/heads/%s\n", file_name.data);
    print_author(revision, &git_date);
    printf("committer Jon Doe <jon.doe@example.com> now\n");
    if(!stringIsEmpty(&revision->comment))
    {
        printf("data %d\n%s\n", stringLength(&revision->comment), revision->comment.data);
    }
    else
    {
        printf("data <<EOF\n/* no comment */\nEOF\n\n");
    }
    if(start_branch)
    {
        printf("from refs/tags/import_initial\n");
    }
}

void start_blob(struct RevData const* revision)
{
    printf("blob\nmark :%d\ndata <<EOF_%p_PAGE\n", revision->blobref, revision);
}

void stop_blob(struct RevData const* revision)
{
    printf("\nEOF_%p_PAGE\n\n", revision);
}
