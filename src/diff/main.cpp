#include <stdio.h>
#include <stdlib.h>

#include "logger.h"
#include "diff.h"
#include "file_data.h"
#include "colour.h"
#include "diff_tree.h"

int main(int argc, const char* argv[])
{
    const char* log_filename = "../resources/logger/logger.log";
    
    logger_constructor(log_filename, DEBUG, true);

    if (argc != 2) LOG_ERROR("Not enough argc");
    
    FILE* file_read = fopen(argv[1], "r");
    

    char* buffer = create_buffer(file_read);

    fprintf(stderr, YELLOW_TEXT("%s"), buffer);

    int index = 0;

    Node_t* root = parse(buffer, &index, nullptr);

    generate_dot(root);

    double result = evaluate(root);

    fprintf(stderr, "result %f", result);

    logger_destructor(get_logger());

    free(buffer);
    free_tree(&root);
}