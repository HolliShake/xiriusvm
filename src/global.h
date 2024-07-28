#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <stdarg.h>
#include <inttypes.h>

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
    /**** UTILITES ****/
    int64_t hash_string(const char* str);

    /**** TYPE DEFS ****/
    // context.h
    typedef struct xirius_context_struct XS_context;
    // environment.h
    typedef struct xirius_environment_struct XS_environment;
    typedef struct xirius_environment_cell_struct XS_environment_cell;
    // gc.h
    typedef struct xirius_gc_struct XS_gc;
    // object.h
    typedef struct object_struct object_t;
    typedef struct object_node_struct object_node_t;
    // opcode.h
    typedef enum xirius_opcode_enum XS_opcode;
    typedef struct xirius_instruction_struct XS_instruction;
    // runtime.h
    typedef struct xirius_runtime_struct XS_runtime;
    // store.h
    typedef struct xirius_store_struct XS_store;
    // value.h
    typedef enum xirius_value_type_enum XS_value_type;
    typedef struct xirius_value_struct XS_value;
    typedef XS_value* (*cfunction_t)(XS_context* context, XS_environment* environment, XS_value* args[], int argc);
#endif