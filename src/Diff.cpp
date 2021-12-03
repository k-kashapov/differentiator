#include "Diff.h"
#include "files.h"
#include "DiffDSL.h"

#define OP_CASE(op)                                                             \
    case *#op:                                                                  \
        *value = l_val op r_val;                                                \
        break

#define UNARY_CASE(val, func)                                                   \
    case val:                                                                   \
        *value = func (l_val);                                                  \
        break;

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

    CreateNodeImage (GetRoot (tree), "init_graph.png");

    free (source);

    return OK;
}

TNode *CopyNode (TNode *src, TNode *parent)
{
    if (!src) return NULL;
    TNode *cpy = CreateNode (src->data);
    cpy->type = src->type;
    cpy->parent = parent;

    if (src->left)
    {
        cpy->left  = CopyNode (src->left, cpy);
    }

    if (src->right)
    {
        cpy->right = CopyNode (src->right, cpy);
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

    OpenTexFile ("Result.tex", "wt");

    PrintInitalTree (src_tree);

    OptimizeTree (src_tree, param);

    res_tree->root = DiffNode (src_tree->root, param);
    TreeOk (res_tree);

    OptimizeTree (res_tree, param);

    CreateNodeImage (GetRoot (res_tree), "diffed.png");

    CloseTexFile ();

    return res_tree;
}

TNode *DiffNode (TNode *node, char param)
{
    TNode *result = NULL;

    switch (node->type)
    {
        case TYPE_CONST:
            result = CreateNode (0, TYPE_CONST);
            break;
        case TYPE_VAR:
            if ((int) node->data == param)
            {
                result = CreateNode (1, TYPE_CONST);
            }
            else
            {
                result = CreateNode (0, TYPE_CONST);
            }
            break;
        case TYPE_UNARY: [[fallthrough]];
        case TYPE_OP:
            switch ((int) node->data)
            {
                case '+':
                    result = ADD (DL, DR);
                    break;
                case '-':
                    result = SUB (DL, DR);
                    break;
                case '*':
                    result = ADD (MUL (DL, CR), MUL (CL, DR));
                    break;
                case '/':
                    result = DIV (SUB (MUL (DL, CR), MUL (DR, CL)), POW (CR, CN (2)));
                    break;
                case '^':
                    result = MUL (POW (CL, CR), ADD (MUL (DR, DLN (CL)), DIV (MUL (CR, DL), CL)));
                    break;
                case SIN:
                    result = MUL (DL, DCOS (CL));
                    break;
                case COS:
                    result = MUL (DL, MUL (CN (-1), DSIN (CL)));
                    break;
                case ASIN:
                    result = DIV (DL, POW (SUB (CN (1), POW (CL, CN (2))), CN (0.5)));
                    break;
                case ACOS:
                    result = DIV (MUL (CN (-1), DL), POW (SUB (CN (1), POW (CL, CN (2))), CN (0.5)));
                    break;
                case LN:
                    result = DIV (DL, CL);
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

    while (OptimizeNode (&result, param)) ;

    if (result)
        PrintDiff (node, result, param);

    return result;
}

static int Fact(int n){
     return (n == 0) || (n == 1) ? 1 : n * Fact (n-1);
}

int McLaurinTree (Tree* tree, char param, int degree)
{
    OpenTexFile ("Result.tex", "wt");
    PrintInitalTree (tree);

    double val  = 0;
    IsConstantNode (GetRoot (tree), &val, '\0');
    printf ("%lg + ", val);

    Tree *res_tree = (Tree*) calloc (1, sizeof (Tree));
    if (!res_tree)
    {
        LOG_ERROR ("CREATE TREE FAILED\n");
        return CREATE_TREE_ERR;
    }

    res_tree->root = DiffNode (tree->root, param);
    TreeOk (res_tree);
    OptimizeTree (res_tree, param);

    for (int curr_deg = 0;
         curr_deg < degree;
         curr_deg++)
    {
        PrintInitalTree (res_tree);

        IsConstantNode (GetRoot (res_tree), &val, '\0');
        if (!IS_EQ_APPROX (val, 0))
        {
            printf ("(%lg)x^%d + ", val / Fact (curr_deg + 1), curr_deg + 1);
        }

        TNode *old_tree = res_tree->root;
        res_tree->root  = DiffNode (res_tree->root, param);
        TreeOk (res_tree);
        OptimizeTree (res_tree, param);

        DestructNode (old_tree);
    }

    CloseTexFile ();

    printf ("o(x^%d)\n", degree);
    DestructTree (res_tree);
    return 0;
}

int OptimizeTree (Tree *tree, char param)
{
    int optimized = 0;

    do
    {
        optimized  = OptimizeNode (&tree->root, param);
        TreeOk (tree);
    }
    while (optimized);

    return OK;
}

static int OptimizeDoubleNeg (TNode **node)
{
    if ((*node)->type != TYPE_OP || (int) (*node)->data != '*')
    {
        return 0;
    }

    if ((int) (*node)->left->data != '*' && (int) (*node)->right->data != '*')
    {
        return 0;
    }

    if ((int) (*node)->left->data == -1)
    {
        if ((int) (*node)->right->left->data == -1)
        {
            TNode *old_ptr = *node;
            (*node)->right->right->parent = old_ptr->parent;
            *node = (*node)->right->right;
            DestructNode (old_ptr->left);
            DestructNode (old_ptr->right->left);
            free (old_ptr->right);
            free (old_ptr);
            return 1;
        }

        if ((int) (*node)->right->right->data == -1)
        {
            TNode *old_ptr = *node;
            (*node)->right->left->parent = old_ptr->parent;
            *node = (*node)->right->left;
            DestructNode (old_ptr->left);
            DestructNode (old_ptr->right->right);
            free (old_ptr->right);
            free (old_ptr);
            return 1;
        }
    }

    return 0;
}

int OptimizeNode (TNode **node, char param)
{
    // printf ("Starting Optim: node: %p; data = %lg; type = %d; left = %p; right = %p; parent = %p\n",
    //         *node, (*node)->data, (*node)->type, (*node)->left, (*node)->right, (*node)->parent);

    double val = 0;
    int isConst = IsConstantNode (*node, &val, param);

    // printf ("Is it const? %d\n", isConst);

    if (isConst)
    {
        if ((*node)->type == TYPE_CONST) return 0;

        TNode *parent = (*node)->parent;
        DestructNode (*node);
        TNode *const_node = CreateNode (val, TYPE_CONST);
        const_node->parent = parent;
        *node = const_node;

        return 1;
    }
    else
    {
        if ((*node)->type == TYPE_OP)
        {
            switch ((char) (*node)->data)
            {
                case '+':
                    if (IS_EQ_APPROX((*node)->left->data, 0))
                    {
                        TNode *old_ptr = *node;
                        (*node)->right->parent = old_ptr->parent;
                        *node = (*node)->right;
                        DestructNode (old_ptr->left);
                        free (old_ptr);
                        return 1;
                    }
                    else if (IS_EQ_APPROX((*node)->right->data, 0))
                    {
                        TNode *old_ptr = *node;
                        (*node)->left->parent = old_ptr->parent;
                        *node = (*node)->left;
                        DestructNode (old_ptr->right);
                        free (old_ptr);
                        return 1;
                    }
                    break;
                case '*':
                    [[fallthrough]];
                case '/':
                    if (IS_EQ_APPROX((*node)->left->data,  0) ||
                        IS_EQ_APPROX((*node)->right->data, 0))
                    {
                        DestructNode ((*node)->left);
                        DestructNode ((*node)->right);
                        (*node)->data = 0;
                        (*node)->type = TYPE_CONST;
                        (*node)->left = NULL;
                        (*node)->right = NULL;
                        return 1;
                    }

                    if (IS_EQ_APPROX((*node)->right->data,  1))
                    {
                        TNode *old_ptr = *node;
                        (*node)->left->parent = old_ptr->parent;
                        *node = (*node)->left;
                        DestructNode (old_ptr->right);
                        free (old_ptr);
                        break;
                    }
                    else if (IS_EQ_APPROX((*node)->left->data,  1) &&
                            (char) (*node)->data == '*')
                    {
                        TNode *old_ptr = *node;
                        (*node)->right->parent = old_ptr->parent;
                        *node = (*node)->right;
                        DestructNode (old_ptr->left);
                        free (old_ptr);
                        break;
                    }
                    else
                    {
                        OptimizeDoubleNeg (node);
                    }
                    break;
                default:
                    break;
            }
        }

        int l_opt = 0;
        int r_opt = 0;
        if ((*node)->left)
        {
            l_opt = OptimizeNode (&(*node)->left, param);
        }
        if ((*node)->right)
        {
            r_opt = OptimizeNode (&(*node)->right, param);
        }
        return l_opt || r_opt;
    }

    return 0;
}

int IsConstantNode (TNode *node, double *value, char param)
{
    // printf ("-- Starting isc: node: %p; data = %lg; type = %d; left = %p; right = %p; parent = %p\n",
    //          node, (node)->data, (node)->type, (node)->left, (node)->right, (node)->parent);
    switch (node->type)
    {
        case TYPE_OP:
            if (!node->right || !node->left)
            {
                LOG_ERROR ("No childen nodes in operation: %c; "
                           "left: %p; right: %p\n",
                           , (char) node->data, node->left, node->right);
            }
            else
            {
                double l_val = 0;
                double r_val = 0;
                int l_opt = IsConstantNode (node->left,  &l_val, param);
                int r_opt = IsConstantNode (node->right, &r_val, param);
                if (!l_opt || !r_opt)
                {
                    return 0;
                }
                else
                {
                    switch ((char) node->data)
                    {
                        OP_CASE (+);
                        OP_CASE (-);
                        OP_CASE (*);
                        OP_CASE (/);
                        case '^':
                            *value = pow (l_val, r_val);
                            break;
                        default:
                            LOG_ERROR ("Invalid operation type: %ld; node %p\n",
                                       , node->data, node);
                    }
                    return 1;
                }
            }
            break;
        case TYPE_UNARY:
            if (!node->left)
            {
                int64_t data = (int64_t) node->data;
                LOG_ERROR ("No left node in operation: %s; "
                           "left: %p; right: %p\n",
                           , (char *)&data, node->left, node->right);
            }
            else
            {
                double l_val = 0;
                int l_opt = IsConstantNode (node->left,  &l_val, param);
                if (!l_opt)
                {
                    return 0;
                }
                else
                {
                    switch ((int) node->data)
                    {
                        UNARY_CASE (SIN,  sin);
                        UNARY_CASE (COS,  cos);
                        UNARY_CASE (ASIN, sin);
                        UNARY_CASE (ACOS, sin);
                        UNARY_CASE (LN,   log);
                        default:
                            int64_t data = (int64_t) node->data;
                            LOG_ERROR ("Invalid unary type: %s; node %p\n",
                                        , (char *)&data, node);
                    }
                    return 1;
                }
            }
            break;
        case TYPE_VAR:
            if ((char) node->data == param)
            {
                return 0;
            }
            else
            {
                return 1;
            }
        case TYPE_CONST:
            *value = node->data;
            return 1;
        default:
            LOG_ERROR ("Invalid node type: %d; node %p;\n",
                       , node->type, node);
    }

    return 0;
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
