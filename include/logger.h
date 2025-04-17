#pragma once

#include <stdio.h>
#include <stdbool.h>

typedef enum {
    DEBUG = 1,
    INFO  = 2,
    ERROR = 3
} log_level;

typedef struct Logger_t {
    FILE* file;
    log_level min_level;
    char* base_filename;
    bool auto_flush;
} Logger_t;

Logger_t* logger_constructor(const char* filename, log_level min_level, bool auto_flush);
void logger_destructor(Logger_t* logger_to_destroy);
const char* log_level_to_str(log_level level);
void log_message(Logger_t* logger, log_level level, const char* file, int line, const char* format, ...);

Logger_t* get_logger(void);
void set_logger(Logger_t* logger);

const int SIZE_TIME_STR = 20;
const long MAX_LOG_FILE_SIZE = 1024 * 1024;
const int SIZE_LOG_FILENAME = 100;

#define LOG_DEBUG(format, ...) log_message(get_logger(), DEBUG, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define LOG_INFO(format , ...) log_message(get_logger(), INFO , __FILE__, __LINE__, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) log_message(get_logger(), ERROR, __FILE__, __LINE__, format, ##__VA_ARGS__)
