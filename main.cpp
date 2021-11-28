#include "Diff.h"

int main (int argc, const char **argv)
{
    Config io_config = {};
    GetArgs (argc, argv, &io_config);
    #ifdef LOGGING
        FILE *log_file = OpenLogFile("Diff " __DATE__ __TIME__);
    #endif

    Tree *diff_tree = CreateTree (0);

    BuildTreeFromFile (&io_config, diff_tree);

    DestructTree (diff_tree);

    #ifdef LOGGING
        fclose (log_file);
    #endif

    return 0;
}
