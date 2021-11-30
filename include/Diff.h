#include "Tree.h"
#include "files.h"

enum TYPES
{
    TYPE_CONST = 0x001,
    TYPE_VAR   = 0x002,
    TYPE_OP    = 0x003,
    TYPE_UNARY = 0x004,
};

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
};

int BuildTreeFromFile (Config *io_config, Tree *tree);

void OpenGraphFile (const char *name);

void CloseGraphFile (void);

void PrintNodeDot (TNode *node);

TNode *CopyNode (TNode *src);

Tree *DiffTree (Tree *src_tree, char param = 'x');

TNode *DiffNode (TNode *node, char param = 'x');

int DisplacementHash (const void *data, size_t len);

void OpenTexFile (const char *name);

void PrintInitalTree (Tree *tree);

void PrintDiff (TNode *before, TNode *after, char param);

void PrintNodeTex (TNode *node);

void CloseTexFile (void);
