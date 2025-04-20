#include <assert.h>

#include "diff_tree.h"
#include "diff.h"
#include "logger.h"

Node_t* diff(Node_t* root)
{
    assert(root);

    switch (root->type)
    {
        case NUM:
            return create_node(NUM, 0, nullptr, nullptr);

        case VAR:
            return create_node(NUM, 1, nullptr, nullptr);

        case OP:
            switch ((int)root->value)
            {
                case ADD:
                    // d(u + v)/dx = d(u)/dx + d(v)/dx
                    return new_node(OP, ADD, nullptr,
                                    diff(root->left),
                                    diff(root->right));

                case MUL:
                    // d(u * v)/dx = u * d(v)/dx + v * d(u)/dx
                    return new_node(OP, ADD, nullptr,
                                    new_node(OP, MUL, nullptr, CopyTree(root->left), diff(root->right)),
                                    new_node(OP, MUL, nullptr, diff(root->left), CopyTree(root->right)));

                case DIV:
                    // d(u / v)/dx = (d(u)/dx * v - u * d(v)/dx) / (v * v)
                    return new_node(OP, DIV, nullptr,
                                    new_node(OP, SUB, nullptr,
                                             new_node(OP, MUL, nullptr, diff(root->left), CopyTree(root->right)),
                                             new_node(OP, MUL, nullptr, CopyTree(root->left), diff(root->right))),
                                    new_node(OP, MUL, nullptr, CopyTree(root->right), CopyTree(root->right)));

                case SUB:
                    // d(u - v)/dx = d(u)/dx - d(v)/dx
                    return new_node(OP, SUB, nullptr,
                                    diff(root->left),
                                    diff(root->right));

                default:
                    LOG_ERROR("Unknown operator: %d", (int)root->value);
                    return nullptr;
            }

        default:
            LOG_ERROR("Unknown node type: %d", root->type);
            return nullptr;
    }
}