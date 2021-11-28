#include "Tree.h"

const int POISON = 0x42;

Tree *CreateTree (tree_elem value LOG_ARGS_IN(,))
{
    Tree *tree = (Tree*) calloc (1, sizeof (Tree));
    if (!tree)
    {
        LOG_ERROR ("CREATE TREE FAILED\n");
    }

    TNode *init_node = CreateNode (value);
    tree->root = init_node;

    tree->size = 1;

    TreeOk (tree);

    return tree;
}

TNode *GetRoot (Tree *tree LOG_ARGS_IN(,))
{
    return tree->root;
}

TNode *CreateNode (tree_elem value LOG_ARGS_IN(,))
{
    TNode *node_ptr = (TNode *) calloc (1, sizeof (TNode));
    if (!node_ptr)
    {
        LOG_FATAL ("MEM_ALLOC_ERR\n");
    }

    node_ptr->data   = value;
    node_ptr->left   = NULL;
    node_ptr->right  = NULL;
    node_ptr->parent = NULL;

    return node_ptr;
}

TNode *AddNodeLeft (TNode *node, tree_elem value LOG_ARGS_IN(,))
{
    TNode *node_ptr  = CreateNode (value);
    node->left       = node_ptr;
    node_ptr->parent = node;

    return node_ptr;
}

TNode *AddNodeRight (TNode *node, tree_elem value LOG_ARGS_IN(,))
{
    TNode *node_ptr = CreateNode (value);
    node->right     = node_ptr;
    node_ptr->parent = node;

    return node_ptr;
}

void TreeNodePrint (TNode *node LOG_ARGS_IN(,))
{
    printf (TYPE_SPEC, node->data);
    return;
}

TNode *VisitNode (TNode *node, NodeAction pre, NodeAction in, NodeAction post LOG_ARGS_IN(,))
{
    if (!node) return 0;

    if (pre)
        pre(node);

    if (node->left)
    {
        VisitNode (node->left, pre, in, post);
    }

    if (in)
        in (node);
    if (node->right)
    {
        VisitNode (node->right, pre, in, post);
    }

    if (post)
        post (node);

    return 0;
}

int64_t TreeOk (Tree *tree LOG_ARGS_IN(,))
{
    int64_t err = 0;

    err |= NodeOk (GetRoot (tree));

    return err;
}

int64_t NodeOk (TNode *node LOG_ARGS_IN(,))
{
    int64_t err = 0;
    if (!node)
    {
        LOG_ERROR ("BAD DATA PTR at %p\n", , node);
        err |= BAD_PTR;
    }

    if (node->left)
    {
        err |= NodeOk (node->left);
    }

    if (node->right)
    {
        err |= NodeOk (node->right);
    }

    return err;
}

int DestructNode (TNode *node LOG_ARGS_IN(,))
{
    if (node->left)
    {
        DestructNode (node->left);
    }
    node->left = (TNode *) POISON;

    if (node->right)
    {
        DestructNode (node->right);
    }
    node->right = (TNode *) POISON;

    free (node);
    return 0;
}

int DestructTree (Tree *tree LOG_ARGS_IN(,))
{
    DestructNode (GetRoot (tree));
    free (tree);

    return 0;
}
