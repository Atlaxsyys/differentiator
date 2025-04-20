#pragma once

#include <stdio.h>

#include "diff.h"
#include "errors.h"

typedef char* elem_t;

const int MAX_PATH_SIZE     = 50;
const int SIZE_DOT_FILENAME = 60;
const int SIZE_PNG_FILENAME = 60;
const int SIZE_COMMAND      = 120;
const int SIZE_FILENAME     = 20;

struct Node_t
{
    node_type type;

    double value;

    Node_t* left;
    Node_t* right;
    Node_t* parent;
};

typedef struct {
    char* path[MAX_PATH_SIZE];
    int number_of_nodes;
} Path;

Node_t* create_node(node_type type, double value, Node_t* parent);
// Node_t* search_node(Node_t* root, elem_t value);
Node_t* parse(char* buffer, int* index, Node_t* parent);
double evaluate(Node_t* node);
Tree_errors free_tree(Node_t** node);
Tree_errors dump_tree(Node_t* root, FILE* file);
int generate_dot(Node_t* root);
Node_t* diff(Node_t* root);