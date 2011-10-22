#include "filehandling.h"

#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

static void generate_filename(struct OutFile* file)
{
    if(!file->name_template_length)
    {
        file->name_template_length = strlen(file->name_template);
    }
    const int fn_string_len = file->name_template_length + 15;
    setStringMinCapacity(&file->filename_cache, fn_string_len);
    snprintf(file->filename_cache.data, fn_string_len-1, file->name_template,
            file->current_id);
    file->filename_cache.data[fn_string_len-1] = 0;
    file->current_id++;
}

static void gen_fifo(struct OutFile* file)
{
    if(file->make_fifo)
    {
        if(mkfifo(file->filename_cache.data, 0644))
        {
            if(EEXIST == errno)
            {
                /* There is already a file, check if it is a pipe */
                struct stat finfo;
                stat(file->filename_cache.data, &finfo);
                if(!S_ISFIFO(finfo.st_mode))
                {
                    fprintf(stderr, "Existing file «%s» is not a fifo.\n",
                            file->filename_cache.data);
                    exit(1);
                }
            }
            else
            {
                perror("Can't create the desired pipe");
                exit(1);
            }
        }
    }
}

void outfile_advance(struct OutFile* file)
{
    if(file->name_template)
    {
        if(file->out)
        {
            fclose(file->out);
        }
        generate_filename(file);
        gen_fifo(file);
        file->out = fopen(file->filename_cache.data, "wb");
        if(!file->out)
        {
            fprintf(stderr, "Can't open file «%s» for writing.",
                    file->name_template);
            exit(1);
        }
    }
    else
    {
        file->out = stdout;
    }
}

