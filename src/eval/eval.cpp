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
    double right = evaluate(node->right);

    if (node->type == OP)
    {
        switch ((int)node->value)
        {
            case ADD: return left + right;
            case SUB: return left - right;
            case MUL: return left * right;
            case DIV: return left / right;
            default:
                LOG_ERROR("Unknown operator: %d", (int)node->value);
                return 0;
        }
    }

    LOG_ERROR("Unknown node type: %d", node->type);
    return 0;
}