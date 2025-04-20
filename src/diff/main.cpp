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
    if (! file_read) LOG_ERROR("failed open file");

    char* buffer = create_buffer(file_read);

    LOG_DEBUG("buffer: %s", buffer);

    int index = 0;

    Node_t* root = parse(buffer, &index, nullptr);
    if(! root)
    {
        LOG_ERROR("Failed to parse expression");
        return 1;
    }

    generate_dot(root);

    double result = evaluate(root);

    fprintf(stderr, "result %f", result);

    Node_t* deriv = diff(root);
    if (!deriv)
    {
        LOG_ERROR("Failed to differentiate");
        free_tree(&root);

        return 1;
    }
    
    double result_diff = evaluate(deriv);
    fprintf(stderr, "\nresult_diff = %f\n", result_diff);

    generate_dot(deriv);

    logger_destructor(get_logger());

    free(buffer);
    free_tree(&root);
    free_tree(&deriv);
}