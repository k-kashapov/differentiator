#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include "Logs.h"

#define TYPE_SPEC "%lf"
typedef double tree_elem;

const double Epsilon = 1e-5;

#define IS_EQ_APPROX(a, b) (a - b < Epsilon)

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
};

enum EXIT_CODES
{
    OK          = 0x0000,
    BAD_PTR     = 0x0001,
    ORPHAN_NODE = 0x0002,
};

typedef void (*NodeAction) (TNode *);

Tree *CreateTree (tree_elem value);

TNode *GetRoot (Tree *tree);

TNode *CreateNode (tree_elem value, int type = 0, TNode *left = NULL, TNode *right = NULL);

TNode *AddNodeLeft (TNode *node, tree_elem value);

TNode *AddNodeRight (TNode *node, tree_elem value);

void TreeNodePrint (TNode *node);

int GetChildrenCount (TNode *node);

int NodesEqual (TNode *first, TNode *second);

TNode *VisitNode (TNode *node, NodeAction pre, NodeAction in, NodeAction post);

int64_t TreeOk (Tree *tree);

int64_t NodeOk (TNode *node);

int DestructNode (TNode *node);

int DestructTree (Tree *tree);
