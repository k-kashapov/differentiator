#include "Diff.h"

int main (int argc, const char **argv)
{
    Config io_config = {};
    GetArgs (argc, argv, &io_config);
    #ifdef LOGGING
        FILE *log_file = OpenLogFile("Diff " __DATE__ __TIME__);
    #endif

    Tree *diff_tree = CreateTree (0);

    int build_err = BuildTreeFromFile (&io_config, diff_tree);
    if (build_err)
    {
        DestructTree (diff_tree);
        #ifdef LOGGING
            fclose (log_file);
        #endif
        return build_err;
    }

    Tree *diffed = DiffTree (diff_tree, 'x');

    OpenGraphFile ("dotInput.dot");
    VisitNode (GetRoot (diffed), NULL, PrintNodeDot, NULL);
    CloseGraphFile();

    system ("dot dotInput.dot -Tpng -o graphDiff.png");
    system ("eog graphDiff.png");

    DestructTree (diff_tree);
    DestructTree (diffed);

    #ifdef LOGGING
        fclose (log_file);
    #endif

    return 0;
}
