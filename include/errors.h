#pragma once

#include <stdio.h>
#include "colour.h"


enum Tree_errors
{
    SUCCESS          = 0,
    MEMORY_ALLOC_ERR = 1,
    FILE_OPEN_ERR    = 2,
    FILE_CLOSE_ERR   = 3,
    FREAD_ERR        = 4,
    NULLPTR_ERR      = 5,
    SNPRINTF_ERR     = 6,
    NOT_ENOUGH_ARGC  = 7,
    FGETS_ERR        = 8
};

const char* errors_messenger(Tree_errors status);

#define ERROR_MESSAGE(status)                                                                                   \
        {                                                                                                       \
            enum Tree_errors temp = status;                                                                     \
            if (temp != SUCCESS)                                                                                \
            {                                                                                                   \
                fprintf (stderr, "\n" RED_TEXT ("ERROR <%d>:") " %s, " PURPLE_TEXT ("%s: %s: line %d.") "\n",   \
                         temp, errors_messenger(temp), __FILE__, __FUNCTION__, __LINE__);                       \
            }                                                                                                   \
        }
