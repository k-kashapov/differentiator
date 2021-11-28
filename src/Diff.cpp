#include "Diff.h"
#include "files.h"

#define OP_CASE(op)                                                             \
    case *#op:                                                                  \
        result = l_val op r_val;                                                \
        break

static int ProcessChar (const char *target, TNode **curr_node, size_t *curr)
{
    printf ("curr char is %c (%d)\n", *target, *target);
    if (isalnum (*target))
    {
        if (isalpha (*target))
        {
            (*curr_node)->data = (tree_elem) (*target);
            (*curr_node)->type = TYPE_VAR;
        }
        else
        {
            double const_value = 0;
            int bytes_read = 0;

            int read = sscanf (target, "%lf%n", &const_value, &bytes_read);
            printf ("Const value read = %lf\n", const_value);
            if (!read)
            {
                LOG_ERROR ("Const value read err: target = %s",
                            , target);
                return CONST_READ_ERR;
            }

            (*curr_node)->data = const_value;
            (*curr_node)->type = TYPE_CONST;
            *curr += (size_t) bytes_read;
        }
        return OK;
    }

    switch (*target)
    {
        case '(':
            if (!(*curr_node)->left)
            {
                AddNodeLeft (*curr_node, 0);
                *curr_node = (*curr_node)->left;
            }
            else if (!(*curr_node)->right)
            {
                AddNodeRight (*curr_node, 0);
                *curr_node = (*curr_node)->right;
            }
            else
            {
                LOG_ERROR ("Node %p L and R already exist\n",
                            , curr_node);
                return LR_ALREADY_EXIST;
            }
            break;
        case ')':
            *curr_node = (*curr_node)->parent;
            break;
        case '+': [[fallthrough]];
        case '-': [[fallthrough]];
        case '*': [[fallthrough]];
        case '/':
            (*curr_node)->data = (tree_elem) (*target);
            (*curr_node)->type = TYPE_OP;
            break;
        default:
            break;
    }

    return OK;
}

int BuildTreeFromFile (Config *io_config, Tree *tree)
{
    char *source = read_file (io_config->input_file);
    if (!source)
    {
        LOG_ERROR ("in function: %s (%d)\n", , __FUNCTION__, __LINE__);
        return -1;
    }

    size_t src_len = strlen (source);
    printf ("Length = %lu\n", src_len);

    TNode *curr_node = GetRoot (tree);

    size_t curr = 0;

    for (; curr < src_len; curr++)
    {
        int errors = ProcessChar (source + curr, &curr_node, &curr);
        if (errors)
        {
            printf ("Erros were detected during file reading. Stopping...\n");
            break;
        }
    }
    if (curr != src_len)
    {
        free (source);
        return BUILD_FROM_FILE_FAIL;
    }

    OpenGraphFile ("dotInput.dot");
    VisitNode (GetRoot (tree), NULL, PrintNodeDot, NULL);
    CloseGraphFile();

    system ("dot dotInput.dot -Tpng -o graph.png");
    system ("eog graph.png");

    free (source);

    return OK;
}

Tree *DiffTree (Tree *src_tree)
{
    Tree *res_tree = CreateTree (0);



    return res_tree;
}

int DiffNode (TNode *node)
{


    return OK;
}

double Calculate (Tree *tree)
{
    return CalculateNode (GetRoot (tree));
}

double CalculateNode (TNode *node)
{
    double result = 0;

    if (node->type == TYPE_OP)
    {
        double l_val = CalculateNode (node->left);
        double r_val = CalculateNode (node->right);
        switch ((char) node->data)
        {
            OP_CASE (+);
            OP_CASE (-);
            OP_CASE (*);
            OP_CASE (/);
            default:
                LOG_ERROR ("Unknown operation %d\n", , (char) node->data);
                return 0;
        }
    }

    return result;
}
