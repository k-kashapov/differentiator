#include "Diff.h"
#include "files.h"

#define DL  DiffNode (node->left, param)
#define DR  DiffNode (node->right, param)
#define CR  CopyNode (node->right)
#define CL  CopyNode (node->left)
#define ADD(l, r) CreateNode ('+', TYPE_OP, l, r)
#define SUB(l, r) CreateNode ('-', TYPE_OP, l, r)
#define MUL(l, r) CreateNode ('*', TYPE_OP, l, r)
#define DIV(l, r) CreateNode ('/', TYPE_OP, l, r)
#define POW(l, r) CreateNode ('^', TYPE_OP, l, r)

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
        if (isalpha (*target))
        {
            char word[5] = {};
            int bytes_read = 0;
            int read = sscanf (target, "%[^)]%n", word, &bytes_read);

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

    int sym_error = ProcessSymbol (target, curr_node);
    if (sym_error)
    {
        return sym_error;
    }

    printf ("curr data is %c (%lf)\n", (char)(*curr_node)->data, (*curr_node)->data);
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

    res_tree->root = DiffNode (src_tree->root, param);

    TreeOk (res_tree);

    return res_tree;
}

TNode *DiffNode (TNode *node, char param)
{
    switch (node->type)
    {
        case TYPE_CONST:
            return CreateNode (0, TYPE_CONST);
        case TYPE_VAR:
            if ((char) node->data == param)
            {
                return CreateNode (1, TYPE_CONST);
            }
            else
            {
                return CreateNode (0, TYPE_CONST);
            }
        case TYPE_OP:
            switch ((char) node->data)
            {
                case '+':
                    return ADD (DL, DR);
                case '-':
                    return SUB (DL, DR);
                case '*':
                    return ADD (MUL (DL, CR), MUL (CL, DR));
                case '/':
                    return DIV (SUB (MUL (DL, CR), MUL (DR, CL)), MUL (CR, CR));
                case '^':
                    return MUL (MUL (CR, POW (CL, SUB (CR, CreateNode (1, TYPE_CONST)))), DL);
                default:
                    LOG_ERROR ("Invalid operation type: %ld; node %p\n",
                                , node->data, node);
            }
            break;
        default:
            LOG_ERROR ("Invalid node type: %d; node %p",
                        , node->data, node);
    }

    return NULL;
}

int DisplacementHash (const void *data, size_t len)
{
    const char *src  = (const char *) data;
    int result = 0;

    for (size_t i = 0; i < len; i++)
    {
        printf ("letter is %c (%x)\n", src[i], src[i]);
        result += src[i] << (len - i - 1) * 8;
    }
    printf ("hash = %#x\n", (unsigned int) result);

    return result;
}
