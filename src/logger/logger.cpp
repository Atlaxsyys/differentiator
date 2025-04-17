#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include "logger.h"
#include "colour.h"
#include "file_data.h"

static Logger_t* current_logger = nullptr;

static int logger_count = 0;

Logger_t* logger_constructor(const char* log_filename, log_level min_level, bool auto_flush)
{
    assert(log_filename);

    Logger_t* new_logger = (Logger_t*) calloc(1, sizeof(Logger_t));

    new_logger->file = fopen(log_filename, "a");
    new_logger->min_level = min_level;
    new_logger->base_filename = strdup(log_filename);
    new_logger->auto_flush = auto_flush;

    if (! new_logger->file)
    {
        free(new_logger->base_filename);
        free(new_logger);
        
        return nullptr;
    }
    
    fprintf(new_logger->file, "\n       ===== New Session Started =====\n");
    set_logger(new_logger);

    return new_logger;
}

static Logger_t* create_new_logger(const char* base_filename, log_level min_level)
{
    assert(base_filename);

    char log_filename[SIZE_LOG_FILENAME] = {};
    logger_count++;
    snprintf(log_filename, sizeof(log_filename), "%s_%d.log", base_filename, logger_count);
    
    return logger_constructor(log_filename, min_level, false);
}

void logger_destructor(Logger_t* logger_to_destroy)
{
    assert(logger_to_destroy);

    if (logger_to_destroy)
    {
        if (logger_to_destroy->file)
        {
            if (logger_to_destroy->auto_flush)  fflush(logger_to_destroy->file);

            fclose(logger_to_destroy->file);
        }

        if (logger_to_destroy->base_filename)
        {
            free(logger_to_destroy->base_filename);
        }

        free(logger_to_destroy);
    }
    
    if (current_logger == logger_to_destroy)
    {
        set_logger(nullptr);
    }
}

const char* log_level_to_str(log_level level)
{
    switch (level)
    {
        case DEBUG:    return "DEBUG";
        case  INFO:    return "INFO";
        case ERROR:    return "ERROR";
        default:       return "UNKNOWN";
    }
}

void log_message(Logger_t* logger, log_level level, const char* file, int line, const char* format, ...)
{
    assert(logger);

    long log_file_size = size_text_file(logger->file);

    if (log_file_size > MAX_LOG_FILE_SIZE)
    {
        Logger_t* new_logger = create_new_logger(logger->base_filename, logger->min_level);

        if (new_logger)
        {
            logger_destructor(logger);
            logger = new_logger;
        }
    }

    time_t current = time(NULL);
    char time_str[SIZE_TIME_STR] = {};
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&current));

    fprintf(logger->file, "[%s] [%s] [%s:%d] ", time_str, log_level_to_str(level), file, line);

    va_list args = {};
    va_start(args, format);
    vfprintf(logger->file, format, args);
    va_end(args);

    fprintf(logger->file, "\n");

    if (logger->auto_flush)    fflush(logger->file);
}

Logger_t* get_logger(void)
{
    return current_logger;
}

void set_logger(Logger_t* new_logger)
{
    current_logger = new_logger;
}
