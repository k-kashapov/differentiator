#include "Diff.h"
#include <time.h>

static FILE *TexFile = NULL;

#define PL PrintNodeTex (node->left)
#define PR PrintNodeTex (node->right)

const char *Transitions[] =
{
    "Очевидно, что",
    "Нетрудно заметить, что",
    "Как говорил мой дед:",
    "Как гласит старая русская поговорка:",
    "Древняя китайская мудрость:",
    "Cразу же на ум приходит такой шаг:",
    "Вроде бы, дальше там так:"
};

const int TR_NUM = sizeof (Transitions) / sizeof (char*);

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
                      "\\begin{document}\n");

    return;
}

void CloseTexFile (void)
{
    if (TexFile)
    {
        fprintf (TexFile, "\n\\end{document}\n");
        fclose (TexFile);
        TexFile = NULL;
    }

    system ("pdflatex -interaction=batchmode Result.tex");
    return;
}

void PrintInitalTree (Tree *tree)
{
    fprintf (TexFile, "\\textbf{Глава 1. Производная сложной функции}\n\n"
                        "Задача: найти производную функции:\n\\[");
    PrintNodeTex (GetRoot (tree));

    fprintf (TexFile, "\\]\n\nПриступим!\n\\\\\n");

    return;
}

void PrintDiff (TNode *before, TNode *after, char param)
{
    fprintf (TexFile, "%s\\[\\frac{d}{d%c} ( ",
                      Transitions[(unsigned) (clock() + rand()) % TR_NUM],
                      param);
    PrintNodeTex (before);
    fprintf (TexFile, " ) = ");
    PrintNodeTex (after);
    fprintf (TexFile, "\\]\n");

    return;
}

void PrintNodeTex (TNode *node)
{
    switch (node->type)
    {
        case TYPE_OP:
            {
                switch ((int) node->data)
                {
                    case '+': [[fallthrough]];
                    case '-':
                        {
                            fprintf (TexFile, "(");
                            PL;
                            fprintf (TexFile, "%c", (char) node->data);
                            PR;
                            fprintf (TexFile, ")");
                        }
                        break;
                    case '^':
                        {
                            PL;
                            fprintf (TexFile, "^{(");
                            PR;
                            fprintf (TexFile, ")}");
                        }
                        break;
                    case '*':
                        {
                            PL;
                            fprintf (TexFile, " \\cdot ");
                            PR;
                        }
                        break;
                    case '/':
                        {
                            fprintf (TexFile, "\\frac{");
                            PL;
                            fprintf (TexFile, "}{");
                            PR;
                            fprintf (TexFile, "} ");
                        }
                        break;
                    default:
                        LOG_ERROR ("TEX: Invalid operation: %lf, node %p\n",
                                    , node->data, node);
                }
            }
            break;
        case TYPE_UNARY:
            {
                int64_t data = (int64_t) node->data;
                fprintf (TexFile, "%s(", (char *) &data);
                PL;
                fprintf (TexFile, ")");
            }
            break;
        case TYPE_VAR:
            fprintf (TexFile, "%c", (char)(node->data));
            break;
        case TYPE_CONST:
            fprintf (TexFile, "%lg", node->data);
            break;
        default:
            LOG_ERROR ("TEX: Invalid node type: %d, node %p\n",
                        , node->type, node);
    }

    return;
}
