#include "Diff.h"
#include <time.h>

static FILE *TexFile = NULL;
const int Max_Len    = 20;

#define PL PrintNodeTex (node->left,  long_nodes, greek_num)
#define PR PrintNodeTex (node->right, long_nodes, greek_num)

const char *Transitions[] =
{
    "Очевидно, что",
    "Нетрудно заметить, что",
    "Как говорил мой дед:",
    "Как гласит старая русская поговорка:",
    "Древняя китайская мудрость:",
    "Cразу же на ум приходит такой шаг:",
    "По-моему, вот так должно быть:",
    "Мне только что позвонили из правительства и подсказали, что",
    "Этот приём я видел на семинаре у Овчоса:",
    "\nЗаходит однажды в бар улитка и говорит:\n\n"
    "- Можно виски с колой?\n\n"
    "- Простите, но мы не обслуживаем улиток.\n\n"
    "И бармен вышвырнул ее за дверь.\n\n"
    "Через неделю заходит опять эта улитка и говорит:"
};

const int TR_NUM = sizeof (Transitions) / sizeof (char*);

const char *Greek[] =
{
    "\\alpha", "\\beta",
    "\\gamma", "\\Delta",
    "\\delta", "\\epsilon",
    "\\zeta", "\\eta",
    "\\Theta", "\\theta",
    "\\iota", "\\kappa",
    "\\Lambda", "\\lambda",
    "\\mu", "\\nu",
    "\\Xi", "\\xi",
    "\\Pi", "\\pi",
    "\\varpi", "\\Sigma",
    "\\sigma", "\\tau",
    "\\Phi", "\\phi",
    "\\varphi", "\\chi",
    "\\Psi", "\\psi",
    "\\Omega", "\\omega"
};

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
    int   greek_num    = 0;
    TNode **long_nodes = (TNode **) calloc (32, sizeof (TNode *));

    srand ((unsigned) clock());
    int rand_num = rand();
    fprintf (TexFile, "%s\\[\\frac{d}{d%c} ( ",
                      Transitions[rand_num % TR_NUM],
                      param);


    PrintNodeTex (before, long_nodes, &greek_num);
    fprintf (TexFile, " ) = ");

    PrintNodeTex (after, long_nodes, &greek_num);

    fprintf (TexFile, "\\]\n%s", greek_num > 0 ? "где\n" : "");

    for (int node = 0; node < greek_num; node++)
    {
        fprintf (TexFile, "\\[ %s = ", Greek[node]);
        PrintNodeTex (long_nodes[node]);
        fprintf (TexFile, "\\]\n");
    }

    free (long_nodes);

    return;
}

static TNode *GetChildWithLessChildrenThan (TNode *node, int child_num)
{
    if (GetChildrenCount (node) > child_num)
    {
        return GetChildWithLessChildrenThan (node->left, child_num);
    }

    return node;
}

void PrintNodeTex (TNode *node, TNode **long_nodes, int *greek_num)
{
    if (long_nodes)
    {
        for (int check_node = 0; check_node < *greek_num; check_node++)
        {
            if (NodesEqual (long_nodes[check_node], node))
            {
                fprintf (TexFile, " %s ", Greek[check_node]);
                return;
            }
        }

        int children = GetChildrenCount (node);
        if (children > Max_Len)
        {
            long_nodes[*greek_num] = GetChildWithLessChildrenThan (node, Max_Len);
            (*greek_num)++;
        }
    }

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
