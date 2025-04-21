#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "diff_tree.h"
#include "colour.h"
#include "logger.h"
#include "diff.h"
#include "errors.h"

#ifdef DEBUG_AKINATOR
    #define ON_DEBUG(...) __VA_ARGS__
#else
    #define ON_DEBUG(...)
#endif

static bool is_number(char* token);
static char* get_token(char* buffer, int* index);
static void skip_spaces_brakets(char* buffer, int* index);

Node_t* create_node(node_type type, double value, char* var_name, Node_t* parent)
{
    Node_t* node = (Node_t*) calloc(1, sizeof(Node_t));

    if (! node)
    {
        LOG_ERROR("node = nullptr!");
        return nullptr;
    }

    node->type = type;
    node->value = value;
    node->var_name = var_name ? strdup(var_name) : nullptr;

    node->left = nullptr;
    node->right = nullptr;

    node->parent = parent;

    return node;
}

Node_t* new_node(node_type type, double value, char* var_name, Node_t* left, Node_t* right)
{
    Node_t* node = (Node_t*) calloc(1, sizeof(Node_t));

    if (! node)
    {
        LOG_ERROR("node = nullptr!");
        return nullptr;
    }

    node->type = type;
    node->value = value;
    node->var_name = var_name ? strdup(var_name) : nullptr;

    node->left = left;
    node->right = right;

    if (left) left->parent = node;
    if (right) left->parent = node;

    return node;
}

Node_t* CopyTree(Node_t* root)
{
    assert(root);

    Node_t* node = new_node(root->type, root->value, root->var_name, nullptr, nullptr);

    if (root->left)   node->left  = CopyTree(root->left);
    if (root->right)  node->right = CopyTree(root->right);

    return node;
}

// Node_t* search_node(Node_t* root, elem_t data)
// {
//     ON_DEBUG( (stderr, "search: root = [%p]\n", root); )

//     ON_DEBUG( (stderr, "after <root==nullptr> : root[%p] >> root-data = '%s' << look for: '%s'\n", root, root->data, data); )
//     if(data == nullptr) fprintf(stderr, RED_TEXT("DAMFDKFLKLFDF"));
//     if (strcmp(root->data, data) == 0)
//     {
//         ON_DEBUG( (stderr, PURPLE_TEXT("\nI GOT SUKA EGO >>> [%p]: '%s'\n\n"),
//                             root, root->data); )
//         return root;
//     }

//     if (root->left)
//     {
//         Node_t* root_l = search_node(root->left, data);  

//         if (root_l != nullptr)  { return root_l; }
//     }
    
//     if (root->right)
//     {
//         Node_t* root_r = search_node(root->right, data);

//         if (root_r != nullptr)  { return root_r; }
//     }

//     return nullptr;
// }

Node_t* parse(char* buffer, int* index, Node_t* parent)
{
    assert(buffer);
    assert(index);

    skip_spaces_brakets(buffer, index);

    char* token = get_token(buffer, index);

    LOG_DEBUG("token: %s", token);

    Node_t* node;

    if (is_number(token))
    {
        node = create_node(NUM, atof(token), nullptr, parent);
        free(token);
        skip_spaces_brakets(buffer, index);
        return node;
    }

    if (isalpha(token[0]) && token[1] == '\0')
    {
        node = create_node(VAR, 0, token, parent);
        free(token);
        skip_spaces_brakets(buffer, index);
        return node;
    }

    double op_value = 0;
    bool is_unary = false;
    if (strcmp(token, "+") == 0) op_value = ADD;
    else if (strcmp(token, "-") == 0) op_value = SUB;
    else if (strcmp(token, "*") == 0) op_value = MUL;
    else if (strcmp(token, "/") == 0) op_value = DIV;
    else if (strcmp(token, "sin") == 0) { op_value = SIN; is_unary = true; }
    else if (strcmp(token, "cos") == 0) { op_value = COS; is_unary = true; }
    else if (strcmp(token, "log") == 0) { op_value = LN;  is_unary = true; }
    else
    {
        LOG_ERROR("Unknown operator: %s", token);
        free(token);
        return nullptr;
    }

    node = create_node(OP, op_value, nullptr, parent);
    free(token);

    node->left = parse(buffer, index, node);
    if (!is_unary)
        node->right = parse(buffer, index, node);

    return node;
}

bool is_number(char* token)
{
    assert(token);

    if (token[0] == '\0') return false;

    int start = 0;
    if (token[0] == '-') start = 1;

    bool hasDigit = false;
    bool hasDot   = false;

    for (int i = start; token[i]; i++)
    {
        if (isdigit(token[i]))
        {
            hasDigit = true;
        }
        else if (token[i] == '.')
        {
            if (hasDot) return false;
            hasDot = true;
        }
        else
        {
            return false;
        }
    }

    return hasDigit;
}

char* get_token(char* buffer, int* index)
{
    assert(buffer);
    assert(index);

    char* token = (char*) calloc(32, sizeof(char));

    int i = 0;
    while (!isspace(buffer[*index]) && buffer[*index] != ')' && buffer[*index] != '\0')
    {
        token[i] = buffer[*index];
        i++;
        (*index)++;
    }

    token[i] = '\0';

    return token;
}

void skip_spaces_brakets(char* buffer, int* index)
{
    assert(buffer);
    assert(index);

    while(isspace(buffer[*index]) || buffer[*index] == '(' || buffer[*index] == ')') (*index)++;
}

Tree_errors free_tree(Node_t** node)
{
    if (*node == nullptr) { return SUCCESS; }

    free_tree(&((*node)->left));
    free_tree(&((*node)->right));

    if ((*node)->var_name) free((*node)->var_name);

    free(*node);

    *node = nullptr;

    return SUCCESS;
}

Tree_errors dump_tree(Node_t* root, FILE* file)
{
    assert(root);

    char data[32];

    if (root->type == NUM)
    {
        snprintf(data, sizeof(data), "%.2f", root->value);
    }

    else if (root->type == VAR)
    {
        strncpy(data, root->var_name ? root->var_name : "?", sizeof(data));
        data[sizeof(data) - 1] = '\0';
    }

    else if (root->type == OP)
    {
        switch ((int)root->value)
        {
            case ADD: strcpy(data, "+");   break;
            case SUB: strcpy(data, "-");   break;
            case MUL: strcpy(data, "*");   break;
            case DIV: strcpy(data, "/");   break;
            case SIN: strcpy(data, "sin"); break;
            case COS: strcpy(data, "cos"); break;
            case  LN: strcpy(data, "log"); break;
            default:  strcpy(data, "???"); break;
        }
    }

    else
    {
        strcpy(data, "???");
    }

    fprintf(file,
            "    \"%p\" [shape=Mrecord, style=filled, fillcolor=\"#F0C0F0\", label=\"{"
            "data: %s | "
            "current: %p | "
            "{ Left: %p | "
            "Right: %p }"
            "}\"];\n",
            root,
            data,
            root,
            root->left,
            root->right);

    if (root->left)
    {
        fprintf(file, "    \"%p\" -> \"%p\";\n", root, root->left);
        dump_tree(root->left, file);
    }

    if (root->right)
    {
        fprintf(file, "    \"%p\" -> \"%p\";\n", root, root->right);
        dump_tree(root->right, file);
    }

    return SUCCESS;
}

int generate_dot(Node_t* root)
{
    assert(root);

    static int file_counter = 0;

    char* dot_filename = (char*) calloc(SIZE_DOT_FILENAME, sizeof(char));
    if (! dot_filename) {
        ERROR_MESSAGE(MEMORY_ALLOC_ERR) }

    char* png_filename = (char*) calloc(SIZE_PNG_FILENAME, sizeof(char));
    if(! png_filename) {
        ERROR_MESSAGE(MEMORY_ALLOC_ERR) }

    char* command      = (char*) calloc(SIZE_COMMAND,      sizeof(char));
    if(! command) {
        ERROR_MESSAGE(MEMORY_ALLOC_ERR) }

    int written_first  = snprintf(dot_filename, SIZE_DOT_FILENAME, "../resources/graph_dump/graph_%d.dot", file_counter);
    int written_second = snprintf(png_filename, SIZE_PNG_FILENAME, "../resources/graph_dump/graph_%d.png", file_counter);

    if (written_first <  0) ERROR_MESSAGE(SNPRINTF_ERR)
    if (written_second < 0) ERROR_MESSAGE(SNPRINTF_ERR)

    FILE* file = fopen(dot_filename, "w");
    if(! file) {
        ERROR_MESSAGE(FILE_OPEN_ERR) }
    
    fprintf(file, "digraph BinaryTree {\n");
    fprintf(file, "    bgcolor=\"#C0C0C0\";\n\n");
    fprintf(file, "    node [shape=record];\n");
    
    dump_tree(root, file);
    
    fprintf(file, "}\n");

    if(fclose(file) != 0) {
        ERROR_MESSAGE(FILE_CLOSE_ERR) }

    int written_third = snprintf(command, SIZE_COMMAND, "dot -Tpng %s -o %s", dot_filename, png_filename);
    if (written_third < 0) {
        ERROR_MESSAGE(SNPRINTF_ERR) }

    system(command);    

    file_counter++;
    
    free(dot_filename);
    free(png_filename);
    free(command);

    return (file_counter - 1);
}