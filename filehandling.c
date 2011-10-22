#include "filehandling.h"

#include <stdlib.h>

void outfile_advance(struct OutFile* file)
{
    if(file->name_template)
    {
        if(file->out)
        {
            fclose(file->out);
        }
        file->out = fopen(file->name_template, "wb");
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

