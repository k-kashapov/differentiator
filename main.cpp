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

    printf ("Что вы хотите сделать?\n1) Производная; 2) Маклорен\n\t");

    char ans[2] = {};
    scanf ("%1[0-9]", ans);

    switch (*ans)
    {
        case '1':
            {
                printf ("А по какой переменной?\n\t");
                scanf ("%1[a-z]", ans);
                printf ("ans is %c (%d)\n", *ans, *ans);

                Tree *diffed = DiffTree (diff_tree, *ans);
                printf ("Чекай ответ в файлике\n");
                DestructTree (diffed);
            }
            break;
        case '2':
            {
                printf ("До какой степени?\n\t");
                int degree = 0;

                if (!scanf ("%d", &degree))
                {
                    printf ("Неверное значение степени\n");
                    break;
                }

                McLaurinTree (diff_tree, 'x', degree);
            }
            break;
        default:
            break;
    }

    DestructTree (diff_tree);

    #ifdef LOGGING
        fclose (log_file);
    #endif

    return 0;
}
