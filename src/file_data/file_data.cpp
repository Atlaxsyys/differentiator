#include <assert.h>
#include <stdlib.h>
#include <ctype.h>

#include "file_data.h"
#include "logger.h"

long size_text_file(FILE* file_read)
{
    assert(file_read);

    fseek(file_read, 0, SEEK_END);
    long size_file = ftell(file_read);
    fseek(file_read, 0, SEEK_SET);

    return size_file;
}

char* create_buffer(FILE* file_read)
{
    assert(file_read);

    long size_file = size_text_file(file_read);

    char* text_buffer = (char*) calloc((size_t) size_file + 1, sizeof(char));
    if(! text_buffer) LOG_ERROR("text_buffer = nullptr!"); 

    size_t number_of_reads = fread(text_buffer, sizeof(char), (size_t) size_file + 1, file_read);
    if (number_of_reads != (size_t) size_file) LOG_ERROR("fread error: number_of_reads != size_file");

    rewind(file_read);

    return text_buffer;
}

size_t n_string(char* commands_buffer, long size_command)
{
    assert(commands_buffer);

    size_t counter_string = 0;

    for (int i = 0; i < size_command; i++)
    {
        if (commands_buffer[i] == '\n')
        {
            counter_string++;
        }
    }

    return counter_string;
}

char** create_string_buffer(char* text_buffer, FILE* file_read)
{
    assert(text_buffer);
    assert(file_read);
    
    long size_file = size_text_file(file_read);
    size_t number_of_lines = n_string(text_buffer, size_file);

    char** string_buffer = (char**) calloc(number_of_lines + 1, sizeof(char*));
    if(! string_buffer) LOG_ERROR("memory allocation error(string_buffer)");

    int line_number = 1;
    
    string_buffer[0] = &(text_buffer[0]);

    for (int i = 0; i < size_file; i++)
    {
        if (text_buffer[i] == '\n')
        {
            text_buffer[i] = '\0';
            string_buffer[line_number] = &(text_buffer[i + 1]);

            line_number++;
        }
    }

    return string_buffer;
}