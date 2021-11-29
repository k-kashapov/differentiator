#include "Diff.h"

static FILE *Graph_file = NULL;

void OpenGraphFile (const char *name)
{
    Graph_file = fopen (name, "wt");
    if (!Graph_file)
    {
        LOG_ERROR ("Cannot open Graph_file, name = %s\n",
                    , name);
        return;
    }

    fprintf (Graph_file, "digraph G\n{rankdir = \"TB\";\nsplines = true;\n");

    return;
}

void CloseGraphFile (void)
{
    if (Graph_file)
    {
        fprintf (Graph_file, "}\n");
        fclose (Graph_file);
        Graph_file = NULL;
    }
    return;
}

static void LinkTreeNodes (TNode *src)
{
    if (src->left)
        fprintf (Graph_file, "NODE%p:sw->NODE%p:n ["
                             "minlen = \"2\"]\n", src, src->left);
    if (src->right)
        fprintf (Graph_file, "NODE%p:se->NODE%p:n ["
                             "minlen = \"2\"]\n", src, src->right);

    return;
}

void PrintNodeDot (TNode *node)
{
    const char *shape = "rectangle";
    const char *color = "red";

    switch (node->type)
    {
        case TYPE_CONST:
            color = "purple";
            break;
        case TYPE_VAR:
            color = "pink";
            shape = "ellipse";
            break;
        case TYPE_OP:
            shape = "diamond";
            switch ((int) node->data)
            {
                case '+':
                    color = "lightgreen";
                    break;
                case '-':
                    color = "cornflowerblue";
                    break;
                case '*':
                    color = "gold";
                    break;
                case '/':
                    color = "orange";
                    break;
                case '^':
                    color = "aqua";
                    break;
                default:
                    LOG_ERROR ("Graph build: Invalid operation: %lf, node %p\n",
                                , node->data, node);
            }
            break;
        default:
            LOG_ERROR ("Graph build: Invalid node type: %d, node %p\n",
                        , node->type, node);
    }
    fprintf (Graph_file,
                "NODE%p"
                "["
                    "shape=%s, style = \"rounded,filled\", "
                    "fillcolor=\"%s\", "
                    "label = \"",
                    node, shape, color);

    switch (node->type)
    {
        case TYPE_OP:  [[fallthrough]];
        case TYPE_VAR:
            fprintf (Graph_file, "%c", (char) node->data);
            break;
        case TYPE_CONST:
            fprintf (Graph_file, "%.3lf\n", node->data);
            break;
        default:
            LOG_ERROR ("Graph build: Invalid node type: %d, node %p\n",
                        , node->type, node);
    }

    fprintf (Graph_file, "\"]\n");

    LinkTreeNodes (node);

    return;
}
