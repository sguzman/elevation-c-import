#include "filehandling.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

static void generate_filename(struct OutFile* file, char which)
{
    /* TODO: check if there is really on %c in the file name */
    if(!file->name_template_length)
    {
        file->name_template_length = strlen(file->name_template);
    }
    /* No need to reserve more space than the template, since the template is
     * always larger than the result */
    const int fn_string_len = file->name_template_length; 
    setStringMinCapacity(&file->filename_cache, fn_string_len);
    snprintf(file->filename_cache.data, fn_string_len-1, file->name_template,
             which);
    file->filename_cache.data[fn_string_len-1] = 0;
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

const int az = 'Z' - 'A';

void files_open(struct OutFile* of)
{
    int i;
    const size_t handle_array_size = sizeof(of->targets)/sizeof(*of->targets);
    assert((handle_array_size - 2) == az);
    for(i=0; i < handle_array_size; ++i)
    {
        if(of->name_template)
        {
            /* Open files A-Z, then _, then m*/
            const char fn_char = (i < az) ? 'A' + i: ((i==az) ? '_' : 'm');
            
            generate_filename(of, fn_char);
            gen_fifo(of);
            of->targets[i] = fopen(of->filename_cache.data, "wb");
            if(!of->targets[i])
            {
                fprintf(stderr, "Can't open file «%s» for writing.",
                        of->name_template);
                exit(1);
            }
        }
        else
        {
            of->targets[i] = stdout;
        }
    }
}

void files_close(struct OutFile* of)
{
    const size_t handle_array_size = sizeof(of->targets)/sizeof(*of->targets);
    if(of->name_template)
    {
        /* Close only files when the output is not stdout. */
        int i;
        for(i=0; i<handle_array_size; ++i)
        {
            if(of->targets[i])
            {
                fclose(of->targets[i]);
                of->targets[i] = NULL;
            }
        }
    }
}

static char page_character(struct DynString const* page_title)
{
    char const* walker=page_title->data;
    bool got_colon = false;
    for(;*walker; ++walker)
    {
        if(got_colon)
        {
            if(!isspace(*walker))
            {
                return *walker;
            }
        }
        else
        {
            if(':' == *walker)
            {
                got_colon = true;
            }
        }
    }
    return *page_title->data;
}

static int convert_char(char in)
{
    const char in_up = toupper(in);
    if((in_up < 'A')||(in_up > 'Z'))
    {
        return az+1;
    }
    return in_up-az;
}

FILE* files_get_page(struct OutFile* of, struct DynString const* page_title)
{
    const int page_char = convert_char(page_character(page_title));
    const size_t handle_array_size = sizeof(of->targets)/sizeof(*of->targets);
    /* The page must be in [A-Z_]*/
    assert(page_char < (handle_array_size - 1));
    return of->targets[page_char];
}

FILE* get_meta_file(struct OutFile* of)
{
    const size_t handle_array_size = sizeof(of->targets)/sizeof(*of->targets);
    return of->targets[handle_array_size-1];
}

void files_close_meta(struct OutFile* of)
{
    const size_t handle_array_size = sizeof(of->targets)/sizeof(*of->targets);
    fclose(of->targets[handle_array_size-1]);
    of->targets[handle_array_size-1] = NULL;
}
