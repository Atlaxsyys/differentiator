#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

#include "diff.h"
#include "logger.h"
#include "diff_tree.h"

static Node_t* simplify_node(Node_t* root);
static Node_t* simplify_number_or_var(Node_t* root);
static Node_t* simplify_unary_operator(Node_t* root);
static Node_t* simplify_binary_operator(Node_t* root);
static Node_t* compute_numeric_binary(Node_t* root);
static Node_t* simplify_add(Node_t* root);
static Node_t* simplify_sub(Node_t* root);
static Node_t* simplify_mul(Node_t* root);
static Node_t* simplify_div(Node_t* root);

Node_t* simplify(Node_t* root)
{
    if (!root) return nullptr;

    if (root->left) root->left = simplify(root->left);
    if (root->right) root->right = simplify(root->right);

    return simplify_node(root);
}

static Node_t* simplify_node(Node_t* root)
{
    assert(root);

    if (root->type == NUM || root->type == VAR) return simplify_number_or_var(root);

    if (root->type == OP)
    {
        if (root->value == SIN || root->value == COS || root->value == LN) return simplify_unary_operator(root);

        return simplify_binary_operator(root);
    }

    LOG_ERROR("Unknown node type: %d", root->type);
    return root;
}

Node_t* simplify_number_or_var(Node_t* root)
{
    assert(root);
    assert(root->type == NUM || root->type == VAR);

    return root;
}

Node_t* simplify_unary_operator(Node_t* root)
{
    assert(root);
    assert(root->type == OP);
    assert(root->value == SIN || root->value == COS || root->value == LN);

    if (root->left->type == NUM)
    {
        double value = 0;

             if (root->value == SIN) value = sin(root->left->value);
        else if (root->value == COS) value = cos(root->left->value);
        else if (root->value == LN)
        {
            if (root->left->value <= 0)
            {
                LOG_ERROR("Log of non-positive value: %f", root->left->value);
                return root;
            }

            value = log(root->left->value);
        }

        Node_t* new_node = create_node(NUM, value, nullptr, nullptr);
        free_tree(&root);
        return new_node;
    }

    return root;
}

Node_t* simplify_binary_operator(Node_t* root)
{
    assert(root);
    assert(root->type == OP);

    if (!root->left || !root->right)
    {
        LOG_ERROR("Binary operator missing operand");
        return root;
    }

    Node_t* numeric_result = compute_numeric_binary(root);
    if (numeric_result != root) return numeric_result;

    switch ((int)root->value)
    {
        case ADD: return simplify_add(root);
        case SUB: return simplify_sub(root);
        case MUL: return simplify_mul(root);
        case DIV: return simplify_div(root);
        default:
            LOG_ERROR("Unknown binary operator: %d", (int)root->value);
            return root;
    }
}

static Node_t* compute_numeric_binary(Node_t* root)
{
    assert(root);
    assert(root->type == OP);
    assert(root->left && root->right);

    if (root->left->type != NUM || root->right->type != NUM)
    {
        return root;
    }

    double value = 0;

         if (root->value == ADD) value = root->left->value + root->right->value;
    else if (root->value == SUB) value = root->left->value - root->right->value;
    else if (root->value == MUL) value = root->left->value * root->right->value;
    else if (root->value == DIV)
    {
        if (root->right->value == 0)
        {
            LOG_ERROR("Division by zero in simplification");
            return root;
        }

        value = root->left->value / root->right->value;
    }
    else
    {
        return root;
    }

    Node_t* new_node = create_node(NUM, value, nullptr, nullptr);
    free_tree(&root);

    return new_node;
}

static Node_t* simplify_add(Node_t* root)
{
    assert(root);
    assert(root->type == OP);
    assert(root->value == ADD);
    assert(root->left && root->right);

    if (root->left->type == NUM && root->left->value == 0)
    {
        Node_t* right = root->right;
        root->right = nullptr;
        free_tree(&root);

        return right;
    }
    if (root->right->type == NUM && root->right->value == 0) {
        Node_t* left = root->left;
        root->left = nullptr;
        free_tree(&root);

        return left;
    }
    return root;
}

static Node_t* simplify_sub(Node_t* root)
{
    assert(root);
    assert(root->type == OP);
    assert(root->value == SUB);
    assert(root->left && root->right);

    if (root->right->type == NUM && root->right->value == 0)
    {
        Node_t* left = root->left;
        root->left = nullptr;
        free_tree(&root);

        return left;
    }

    if (root->left->type == NUM && root->left->value == 0)
    {
        Node_t* right = root->right;
        root->right = nullptr;
        free_tree(&root);

        return new_node(OP, MUL, nullptr, create_node(NUM, -1, nullptr, nullptr), right);
    }

    return root;
}

static Node_t* simplify_mul(Node_t* root)
{
    assert(root);
    assert(root->type == OP);
    assert(root->value == MUL);
    assert(root->left && root->right);

    if (root->left->type == NUM && root->left->value == 0)
    {
        free_tree(&root);
        return create_node(NUM, 0, nullptr, nullptr);
    }
    if (root->right->type == NUM && root->right->value == 0)
    {
        free_tree(&root);
        return create_node(NUM, 0, nullptr, nullptr);
    }
    if (root->left->type == NUM && root->left->value == 1)
    {
        Node_t* right = root->right;
        root->right = nullptr;
        free_tree(&root);
        
        return right;
    }
    if (root->right->type == NUM && root->right->value == 1)
    {
        Node_t* left = root->left;
        root->left = nullptr;
        free_tree(&root);

        return left;
    }
    return root;
}

static Node_t* simplify_div(Node_t* root)
{
    assert(root);
    assert(root->type == OP);
    assert(root->value == DIV);
    assert(root->left && root->right);

    if (root->right->type == NUM && root->right->value == 1)
    {
        Node_t* left = root->left;
        root->left = nullptr;
        free_tree(&root);

        return left;
    }
    return root;
}