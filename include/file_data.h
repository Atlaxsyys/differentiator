#pragma once

#include <stdio.h>

long size_text_file(FILE* file_read);
char* create_buffer(FILE* file_read);
size_t n_string(char* commands_buffer, long size_command);
char** create_string_buffer(char* text_buffer, FILE* file_read);
