#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#ifdef LOGGING
    #define LOG_ARGS_IN(...)  __VA_ARGS__ FILE *log_file, const char *called_from, int line
    #define LOG_ARGS_OUT(...) __VA_ARGS__ log_file, __FUNCTION__, __LINE__
    #define LOG_ARGS_STR "%s (%d) "
#else
    #define LOG_ARGS_IN(...)
    #define LOG_ARGS_OUT(...)
    #define LOG_ARGS_STR
#endif


FILE *OpenLogFile(const char *file_name);

void LogErr (FILE *log_file, const char *fatal, const char *msg, ...);

#ifdef LOGGING
    #define LOG_ERROR(string, ...) LogErr (LOG_ARGS_OUT(), "ERROR", string __VA_ARGS__)
    #define LOG_FATAL(string, ...) LogErr (LOG_ARGS_OUT(), "FATAL", string __VA_ARGS__)
#else
    #define LOG_ERROR(string, ...) LogErr (stderr, "ERROR", string __VA_ARGS__)
    #define LOG_FATAL(string, ...) LogErr (stderr, "FATAL", string __VA_ARGS__)
#endif
