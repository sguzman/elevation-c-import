#include "filehandling.h"

#include <stdlib.h>
#include <string.h>

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

void outfile_advance(struct OutFile* file)
{
    if(file->name_template)
    {
        if(file->out)
        {
            fclose(file->out);
        }
        generate_filename(file);
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

