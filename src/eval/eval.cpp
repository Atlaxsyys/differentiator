#include <math.h>
#include <assert.h>

#include "diff_tree.h"
#include "diff.h"
#include "logger.h"

static double evaluate_operator(double left, double right, int op);

double evaluate(Node_t* node)
{
    assert(node);

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
        return evaluate_operator(left, right, (int) node->value);
    }

    LOG_ERROR("Unknown node type: %d", node->type);
    return 0;
}

double evaluate_operator(double left, double right, int op)
{
    switch (op)
    {
        case ADD: return left + right;
        case SUB: return left - right;
        case MUL: return left * right;
        case DIV:
            if (right == 0)
            {
                LOG_ERROR("Division by zero: %f / %f", left, right);
                return NAN;
            }
            
            return left / right;
        case SIN: return sin(left);
        case COS: return cos(left);
        case LN:
            if (left <= 0)
            {
                LOG_ERROR("Log of non-positive value: %f", left);
                return NAN;
            }

            return log(left);
        default:
            LOG_ERROR("Unknown operator: %d", op);
            return 0;
    }
}