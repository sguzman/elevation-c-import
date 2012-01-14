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
    switch(check_template(name_template))
    {
        case tkNone:
            of->output_mode = omStdout;
        break;

        case tkSingle:
            of->output_mode = omOneFile;
        break;

        case tkReplace:
            of->output_mode = omManyFiles;
        break;

        case tkError:
            fprintf(stderr,
                    "The file name `%s' is not a valid filename template.\n",
                    name_template);
            exit(1);
        break;
    }
    of->name_template = name_template;
    if(name_template)
    {
        of->name_template_length = strlen(name_template);
    }
    of->make_fifo = make_fifo;
}

static void generate_filename(struct OutFile* file, const char* which)
{
    /* No need to reserve more space than the template, since the template is
     * always larger than the result */
    const int fn_string_len = file->name_template_length + strlen(which) + 1; 
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

static FILE* files_open_single(struct OutFile* of, const char* which,
                               bool supress_fifo)
{
    FILE* result = NULL;
    if(omStdout == of->output_mode)
    {
        /* TODO: change stdout to binary on WIN32 */
        return stdout;
    }
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
    FILE** storage = NULL;
    switch(of->output_mode)
    {
        case omStdout:
        case omOneFile:
            storage = &of->files.single;
        break;

        case omManyFiles:
            storage = &of->files.many.meta;
        break;
    }
    assert(storage);
    if(!*storage)
    {
        *storage = files_open_single(of, "meta", true);
    }
}

void files_open_dispatch(struct OutFile* of)
{
    int i;
    const size_t handle_array_size = sizeof(of->files.many.targets)
                                     /sizeof(*of->files.many.targets);
    assert((handle_array_size - 2) == az);
    for(i=0; i < handle_array_size; ++i)
    {
        FILE** storage = NULL;

        switch(of->output_mode)
        {
            case omStdout:
            case omOneFile:
                storage = &of->files.single;
            break;

            case omManyFiles:
                storage = &(of->files.many.targets[i]);
            break;
        }
        assert(storage);
        if(!*storage)
        {
            /* Open files A-Z, then _ */
            const char fn_char[] = {(i <= az) ? 'A' + i: '_', 0};
            *storage = files_open_single(of, fn_char, false);
        }
    }
}

static void close_file(FILE** file)
{
    if(*file)
    {
        fclose(*file);
    }
    *file = NULL;
}

void files_close(struct OutFile* of)
{
    switch(of->output_mode)
    {
        case omStdout:
            /** Do Nothing */
        break;

        case omOneFile:
            close_file(&of->files.single);
        break;

        case omManyFiles:
        {
            const size_t handle_array_size = sizeof(of->files.many.targets)
                                             /sizeof(*of->files.many.targets);
            for(int i=0; i<handle_array_size; ++i)
            {
                close_file(&of->files.many.targets[i]);
            }
        }
        break;
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
    switch(of->output_mode)
    {
        case omStdout:
        case omOneFile:
            return of->files.single;

        case omManyFiles:
        {
            const int page_char = files_convert_char(
                                    files_page_character(page_title));
            const size_t handle_array_size = sizeof(of->files.many.targets)
                                             /sizeof(*of->files.many.targets);
            /* The page must be in [A-Z_]*/
            assert(page_char < handle_array_size);
            return of->files.many.targets[page_char];
        }
    }
    assert("This statement should not be reached" == 0);
    return NULL;
}

FILE* files_get_meta(struct OutFile* of)
{
    switch(of->output_mode)
    {
        case omStdout:
        case omOneFile:
            return of->files.single;

        case omManyFiles:
            return of->files.many.meta;
    }
    assert("This statement should not be reached" == 0);
    return NULL;
}

void files_close_meta(struct OutFile* of)
{
    if(omManyFiles == of->output_mode)
    {
        close_file(&of->files.many.meta);
    }
}
