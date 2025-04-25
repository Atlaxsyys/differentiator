#include <stdio.h>
#include <stdlib.h>

#include "logger.h"
#include "diff.h"
#include "file_data.h"
#include "colour.h"
#include "diff_tree.h"

int main(int argc, const char* argv[])
{
    char* buffer = nullptr;
    Node_t* root = nullptr;
    Node_t* deriv = nullptr;
    int index = 0;
    double result = 0.0;
    double result_diff = 0.0;
    FILE* file_read = nullptr;

    const char* log_filename = "../resources/logger/logger.log";

    if (argc != 2) { LOG_ERROR("not enough argc"); goto cleanup; }
    
    logger_constructor(log_filename, DEBUG, true);
    
    file_read = fopen(argv[1], "r");
    if (! file_read) { LOG_ERROR("Failed to open file: %s", argv[1]); goto cleanup; }

    buffer = create_buffer(file_read);
    LOG_DEBUG("buffer: %s", buffer);
    if (! buffer)
    {
        fclose(file_read);
        goto cleanup;
    }

    if(fclose(file_read) != 0) { LOG_ERROR("Failed to close file: %s", argv[1]); goto cleanup; }

    root = parse(buffer, &index, nullptr);
    if(! root)
    {
        LOG_ERROR("Failed to parse expression");
        goto cleanup;
    }

    generate_dot(root);

    result = evaluate(root);

    fprintf(stderr, "result %f", result);

    deriv = diff(root);
    if (!deriv)
    {
        LOG_ERROR("Failed to differentiate");
        goto cleanup;
    }

    result_diff = evaluate(deriv);
    fprintf(stderr, "\nresult_diff = %f\n", result_diff);

    generate_dot(deriv);
    
cleanup:
    logger_destructor(get_logger());
    free(buffer);
    free_tree(&root);
    free_tree(&deriv);
}