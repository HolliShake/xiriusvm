#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <stdarg.h>

#ifndef GLOBAL_H
#define GLOBAL_H
    #define assert_allocation(ptr)\
        if (ptr == NULL) {\
            fprintf(stderr, "%s::%s[%d]: Memory allocation failed!!!\n", __FILE__, __func__, __LINE__);\
            exit(1);\
        }\

    #define LABEL() printf("<%s::%s[%d] />\n", __FILE__, __func__, __LINE__)

    #if defined(_WIN32) || defined(_WIN64)
        #define EXPORT __declspec(dllexport)
        #define IMPORT __declspec(dllimport)
    #else
        #define EXPORT
        #define IMPORT
    #endif

    /**** MEMORY ****/
    #define XS_malloc(sz) malloc(sz)
    #define XS_realloc(ptr, sz) realloc(ptr, sz)
    #define XS_free(ptr) free(ptr)

    /**** STRINGS ******/
    char* str__new(const char* str);
    char* str__add(char* str0, char* str1);
    char* str__format(const char* format, ...);
    bool str__equals(const char* str0, const char* str1);
#endif