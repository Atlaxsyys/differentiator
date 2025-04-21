#include <math.h>

#include "diff_tree.h"
#include "diff.h"
#include "logger.h"

double evaluate(Node_t* node)
{
    if (node->type == NUM)
    {
        return node->value;
    }

    if (node->type == VAR)
    {
        return 0;
    }

    double left  = evaluate(node->left);
    double right = node->right ? evaluate(node->right) : 0;

    if (node->type == OP)
    {
        switch ((int)node->value)
        {
            case ADD: return left + right;
            case SUB: return left - right;
            case MUL: return left * right;
            case DIV: return left / right;
            case SIN: return sin(left);
            case COS: return cos(left);
            case  LN: return log(left);
            default:
                LOG_ERROR("Unknown operator: %d", (int)node->value);
                return 0;
        }
    }

    LOG_ERROR("Unknown node type: %d", node->type);
    return 0;
}