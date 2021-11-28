#include "Logs.h"

FILE *OpenLogFile(const char *file_name)
{
    FILE *log = fopen (file_name, "wt");
    if (!log)
    {
        printf ("OPEN LOG FILE FAILED");
        return NULL;
    }

    return log;
}

void LogErr (FILE *log_file, const char *fatal, const char *msg, ...)
{
    #ifdef LOGGING
        fprintf (log_file,  "%s (%d) %s: ",
                            called_from, line, fatal);
    #else
        fprintf (log_file,  "%s: ",
                            fatal);
    #endif

    va_list args = {};
    va_start (args, msg);

    vfprintf (log_file, msg, args);

    va_end (args);

    return;
}
