#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include "Logs.h"

#define TYPE_SPEC "%lf"
typedef double tree_elem;

struct TNode
{
    tree_elem data;
    TNode *left;
    TNode *right;
    TNode *parent;
    int type;
};

struct Tree
{
    TNode *root;
    long size;
};

enum EXIT_CODES
{
    OK          = 0x0000,
    BAD_PTR     = 0x0001,
};

typedef void (*NodeAction) (TNode *);

Tree *CreateTree (tree_elem value LOG_ARGS_IN(,));

TNode *GetRoot (Tree *tree LOG_ARGS_IN(,));

TNode *CreateNode (tree_elem value LOG_ARGS_IN(,));

TNode *AddNodeLeft (TNode *node, tree_elem value LOG_ARGS_IN(,));

TNode *AddNodeRight (TNode *node, tree_elem value LOG_ARGS_IN(,));

void TreeNodePrint (TNode *node LOG_ARGS_IN(,));

TNode *VisitNode (TNode *node, NodeAction pre, NodeAction in, NodeAction post LOG_ARGS_IN(,));

int64_t TreeOk (Tree *tree LOG_ARGS_IN(,));

int64_t NodeOk (TNode *node LOG_ARGS_IN(,));

int DestructNode (TNode *node LOG_ARGS_IN(,));

int DestructTree (Tree *tree LOG_ARGS_IN(,));
