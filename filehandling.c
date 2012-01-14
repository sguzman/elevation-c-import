#include "filehandling.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

enum TemplateKind check_template(const char* name_template)
{
    if(!name_template)
    {
        return tkNone;
    }
    {
        char state = 'v';
        /* States: 'v'anilla: no percent sing seen yet
                   'p'ercent: one percent sing seen
                   's':       's' char after percent sign seen */
        /* const char* walker; */
        for(const char* walker = name_template; *walker; walker++)
        {
            switch(state)
            {
                case 'v':
                    if('%' == *walker)
                    {
                        state = 'p';
                    }
                break;

                case 'p':
                    if('s' == *walker)
                    {
                        state = 's';
                    }else{
                        return tkError;
                    }
                break;

                case 's':
                    if('%' == *walker)
                    {
                        return tkError;
                    }
                break;
            }
        }
        if(state == 's')
        {
            return tkReplace;
        }
    }
    return tkSingle;
}

void files_init(struct OutFile* of, char const* name_template, bool make_fifo)
{
    memset(of, 0, sizeof(*of));
    /* TODO: check if there is really one %s in the file name */
    of->name_template = name_template;
    if(name_template)
    {
        of->name_template_length = strlen(name_template);
    }
    of->make_fifo = make_fifo;
}

static void generate_filename(struct OutFile* file, char which)
{
    /* No need to reserve more space than the template, since the template is
     * always larger than the result */
    const int fn_string_len = file->name_template_length + 1; 
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

static FILE* files_open_single(struct OutFile* of, char which, bool supress_fifo)
{
    FILE* result = NULL;
    generate_filename(of, which);
    if(!supress_fifo)
    {
        gen_fifo(of);
    }
    fprintf(stderr, "opening file %s\n", of->filename_cache.data);
    result = fopen(of->filename_cache.data, "wb");
    fprintf(stderr, "Done\n");
    if(!result)
    {
        fprintf(stderr, "Can't open file «%s» for writing.\n",
                of->filename_cache.data);
        exit(1);
    }
    return result;
}

void files_open_meta(struct OutFile* of)
{
    if(of->name_template)
    {
        of->meta = files_open_single(of, 'm', true);
    }
    else
    {
        of->meta = stdout;
    }
}

void files_open_dispatch(struct OutFile* of)
{
    int i;
    const size_t handle_array_size = sizeof(of->targets)/sizeof(*of->targets);
    assert((handle_array_size - 2) == az);
    for(i=0; i < handle_array_size; ++i)
    {
        if(of->name_template)
        {
            /* Open files A-Z, then _ */
            const char fn_char = (i <= az) ? 'A' + i: '_';
            of->targets[i] = files_open_single(of, fn_char, false);
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

char files_page_character(char const* page_title)
{
    char const* walker=page_title;
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
    return *page_title;
}

int files_convert_char(char in)
{
    const char in_up = toupper(in);
    if((in_up < 'A')||(in_up > 'Z'))
    {
        return az+1;
    }
    return in_up-'A';
}

FILE* files_get_page(struct OutFile* of, char const* page_title)
{
    const int page_char = files_convert_char(files_page_character(page_title));
    const size_t handle_array_size = sizeof(of->targets)/sizeof(*of->targets);
    /* The page must be in [A-Z_]*/
    assert(page_char < handle_array_size);
    return of->targets[page_char];
}

FILE* files_get_meta(struct OutFile* of)
{
    return of->meta;
}

void files_close_meta(struct OutFile* of)
{
    if(of->name_template)
    {
        fclose(of->meta);
        of->meta = NULL;
    }
}
