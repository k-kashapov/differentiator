#include "Tree.h"
#include "files.h"

enum TYPES
{
    TYPE_CONST = 0x001,
    TYPE_VAR   = 0x002,
    TYPE_OP    = 0x003,
};

enum UNARY_FUNCS
{
    SIN  = 0x1,
    COS  = 0x2,
    ASIN = 0x3,
    ACOS = 0x4,
    LN   = 0x5,
};

enum DIFF_EXIT_CODES
{
    LR_ALREADY_EXIST     = 0x101,
    CONST_READ_ERR       = 0x102,
    BUILD_FROM_FILE_FAIL = 0x104,
    INCORR_BRACKET_SEQ   = 0x108,
};

int BuildTreeFromFile (Config *io_config, Tree *tree);

void OpenGraphFile (const char *name);

void CloseGraphFile (void);

void PrintNodeDot (TNode *node);

TNode *CopyNode (TNode *src);

Tree *DiffTree (Tree *src_tree, char param = 'x');

TNode *DiffNode (TNode *node, char param = 'x');
