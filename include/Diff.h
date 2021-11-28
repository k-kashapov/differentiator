#include "Tree.h"
#include "files.h"

enum TYPES
{
    TYPE_CONST = 0x001,
    TYPE_VAR   = 0x002,
    TYPE_OP    = 0x003,
};

enum DIFF_EXIT_CODES
{
    LR_ALREADY_EXIST     = 0x0101,
    CONST_READ_ERR       = 0x0102,
    BUILD_FROM_FILE_FAIL = 0x0104,
};

int BuildTreeFromFile (Config *io_config, Tree *tree);

void OpenGraphFile (const char *name);

double Calculate (Tree *tree);

double CalculateNode (TNode *node);

void CloseGraphFile (void);

void PrintNodeDot (TNode *node);
