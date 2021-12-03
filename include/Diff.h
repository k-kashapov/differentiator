#include "Tree.h"
#include "files.h"

// Hash value for unary function = reverse name of the function
enum UNARY_FUNCS
{
    SIN  = 'nis',
    COS  = 'soc',
    ASIN = 'nisa',
    ACOS = 'soca',
    LN   = 'nl',
};

enum DIFF_EXIT_CODES
{
    LR_ALREADY_EXIST     = 0x101,
    CONST_READ_ERR       = 0x102,
    BUILD_FROM_FILE_FAIL = 0x104,
    INCORR_BRACKET_SEQ   = 0x108,
    WORD_READ_ERR        = 0x110,
    CREATE_TREE_ERR      = 0x120,
    INVALID_DEGREE       = 0x140,
};

int BuildTreeFromFile (Config *io_config, Tree *tree);

void OpenGraphFile (const char *name);

void CloseGraphFile (void);

void PrintNodeDot (TNode *node);

TNode *CopyNode (TNode *src, TNode *parent = NULL);

Tree *DiffTree (Tree *src_tree, char param = 'x');

TNode *DiffNode (TNode *node, char param = 'x');

int McLaurinTree (Tree* tree, char param, int degree);

int OptimizeTree (Tree *tree, char param);

int OptimizeNode (TNode **node, char param);

int IsConstantNode (TNode *node, double *value, char param);

int DisplacementHash (const void *data, size_t len);

void OpenTexFile (const char *name, const char *mode);

void PrintInitalTree (Tree *tree);

void PrintDiff (TNode *before, TNode *after, char param);

void PrintNodeTex (TNode *node, TNode **long_nodes = NULL, int *greek_num = 0);

int CreateNodeImage (TNode *node, const char *name);

void CloseTexFile (void);
