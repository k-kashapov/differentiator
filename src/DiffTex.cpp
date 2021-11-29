#include "Diff.h"

static FILE *TexFile = NULL;

void OpenTexFile (const char *name)
{
    TexFile = fopen (name, "wt");
    if (!TexFile)
    {
        LOG_ERROR ("Cannot open TexFile, name = %s\n",
                    , name);
        return;
    }

    fprintf (TexFile, "\\documentclass[a4paper,14pt]{extarticle}\n"
                      "\\usepackage{graphicx}\n"
                      "\\usepackage{ucs}\n"
                      "\\usepackage[utf8x]{inputenc}\n"
                      "\\usepackage[russian]{babel}\n"
                      "\\usepackage{multirow}\n"
                      "\\usepackage{mathtext}\n"
                      "\\usepackage[T2A]{fontenc}\n"
                      "\\usepackage{titlesec}\n"
                      "\\usepackage{float}\n"
                      "\\usepackage{empheq}\n"
                      "\\usepackage{amsfonts}\n"
                      "\\usepackage{amsmath}\n"
                      "\\usepackage{tikz}\n"
                      "\\begin{document}\n\\[");

    return;
}

void CloseTexFile (void)
{
    if (TexFile)
    {
        fprintf (TexFile, "\\]\n\\end{document}\n");
        fclose (TexFile);
        TexFile = NULL;
    }
    return;
}

void PrintNodeTex (TNode *node)
{
    

    return;
}
