#include "Diff.h"
#include "files.h"

#define DL  DiffNode (node->left, param)
#define DR  DiffNode (node->right, param)
#define CR  CopyNode (node->right)
#define CL  CopyNode (node->left)
#define CN(val)    CreateNode (val, TYPE_CONST)
#define ADD(l, r)  CreateNode ('+',  TYPE_OP,    l, r)
#define SUB(l, r)  CreateNode ('-',  TYPE_OP,    l, r)
#define MUL(l, r)  CreateNode ('*',  TYPE_OP,    l, r)
#define DIV(l, r)  CreateNode ('/',  TYPE_OP,    l, r)
#define POW(l, r)  CreateNode ('^',  TYPE_OP,    l, r)
#define DSIN(val)  CreateNode (SIN,  TYPE_UNARY, val)
#define DCOS(val)  CreateNode (COS,  TYPE_UNARY, val)
#define DASIN(val) CreateNode (ASIN, TYPE_UNARY, val)
#define DACOS(val) CreateNode (ACOS, TYPE_UNARY, val)
#define DLN(val)   CreateNode (LN,   TYPE_UNARY, val)

static int ProcessAlNum (const char *target, TNode **curr_node, size_t *curr)
{
    if (isalpha (*target))
    {
        char word[5] = {};
        int bytes_read = 0;
        int read = sscanf (target, "%[^()]%n", word, &bytes_read);

        if (bytes_read < 1 || read == 0)
        {
            LOG_ERROR ("Invalid letters sequence: %s", , target);
            return WORD_READ_ERR;
        }
        else if (bytes_read == 1)
        {
            (*curr_node)->data = (tree_elem) (*target);
            (*curr_node)->type = TYPE_VAR;
        }
        else
        {
            printf ("word scanned = %s\n", word);
            int word_hash = DisplacementHash (word, (size_t) bytes_read);

            (*curr_node)->data = (tree_elem) (word_hash);
            (*curr_node)->type = TYPE_UNARY;

            *curr += (size_t) bytes_read - 1;
        }
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
        *curr += (size_t) bytes_read - 1;
    }

    return OK;
}

static int ProcessSymbol (const char *target, TNode **curr_node)
{
    static int brackets = 0;

    switch (*target)
    {
        case '(':
            brackets++;
            if (!(*curr_node)->left)
            {
                AddNodeLeft (*curr_node, 0);
                *curr_node = (*curr_node)->left;
            }
            else if (!(*curr_node)->right && (*curr_node)->type != TYPE_UNARY)
            {
                AddNodeRight (*curr_node, 0);
                *curr_node = (*curr_node)->right;
            }
            else
            {
                LOG_ERROR ("Node %p L and R already exist: %s\n",
                            , curr_node, target);
                return LR_ALREADY_EXIST;
            }
            break;
        case ')':
            brackets--;
            *curr_node = (*curr_node)->parent;
            break;
        case '^': [[fallthrough]];
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

    if (brackets < 0)
    {
        LOG_ERROR ("Incorrect bracket sequence!\n");
        return INCORR_BRACKET_SEQ;
    }

    return OK;
}

static int ProcessChar (const char *target, TNode **curr_node, size_t *curr)
{
    printf ("---------\ncurr char is %c (%d)\n", *target, *target);
    if (isalnum (*target))
    {
        int alnum_err = ProcessAlNum (target, curr_node, curr);
        if (alnum_err)
        {
            return alnum_err;
        }
    }

    int sym_error = ProcessSymbol (target, curr_node);
    if (sym_error)
    {
        return sym_error;
    }

    printf ("curr data is %c (%lf)\n", (int)(*curr_node)->data, (*curr_node)->data);
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

TNode *CopyNode (TNode *src)
{
    if (!src) return NULL;
    TNode *cpy = CreateNode (src->data);
    cpy->type = src->type;

    if (src->left)
    {
        cpy->left  = CopyNode (src->left);
    }

    if (src->right)
    {
        cpy->right = CopyNode (src->right);
    }

    return cpy;
}

Tree *DiffTree (Tree *src_tree, char param)
{
    Tree *res_tree = (Tree*) calloc (1, sizeof (Tree));
    if (!res_tree)
    {
        LOG_ERROR ("CREATE TREE FAILED\n");
    }

    OpenTexFile ("Result.tex");

    PrintInitalTree (src_tree);

    res_tree->root = DiffNode (src_tree->root, param);
    TreeOk (res_tree);

    CloseTexFile ();

    return res_tree;
}

TNode *DiffNode (TNode *node, char param)
{
    TNode *result = NULL;

    switch (node->type)
    {
        case TYPE_CONST:
            result =  CreateNode (0, TYPE_CONST);
            break;
        case TYPE_VAR:
            if ((int) node->data == param)
            {
                result =  CreateNode (1, TYPE_CONST);
            }
            else
            {
                result =  CreateNode (0, TYPE_CONST);
            }
            break;
        case TYPE_UNARY: [[fallthrough]];
        case TYPE_OP:
            switch ((int) node->data)
            {
                case '+':
                    result =  ADD (DL, DR);
                    break;
                case '-':
                    result =  SUB (DL, DR);
                    break;
                case '*':
                    result =  ADD (MUL (DL, CR), MUL (CL, DR));
                    break;
                case '/':
                    result =  DIV (SUB (MUL (DL, CR), MUL (DR, CL)), POW (CR, CN (2)));
                    break;
                case '^':
                    result =  MUL (POW (CL, CR), ADD (MUL (DR, DLN (CL)), DIV (MUL (CR, DL), CL)));
                    break;
                case SIN:
                    result =  MUL (DL, DCOS (CL));
                    break;
                case COS:
                    result =  MUL (DL, MUL (CN (-1), DSIN (CL)));
                    break;
                case ASIN:
                    result =  DIV (DL, POW (SUB (CN (1), POW (CL, CN (2))), CN (0.5)));
                    break;
                case ACOS:
                    result =  DIV (MUL (CN (-1), DL), POW (SUB (CN (1), POW (CL, CN (2))), CN (0.5)));
                    break;
                case LN:
                    result =  DIV (DL, CL);
                    break;
                default:
                    LOG_ERROR ("Invalid operation type: %ld; node %p\n",
                                , node->data, node);
            }
            break;
        default:
            LOG_ERROR ("Invalid node type: %d; node %p\n",
                        , node->data, node);
    }

    if (result)
        PrintDiff (node, result, param);

    return result;
}

int DisplacementHash (const void *data, size_t len)
{
    const char *src  = (const char *) data;
    int result = 0;

    for (size_t i = 0; i < len; i++)
    {
        result += src[i] << i * 8;
    }
    printf ("hash = %#x\n", (unsigned int) result);

    return result;
}
