#include "global.h"

#ifndef STORE_H
#define STORE_H
    typedef struct xirius_store_struct {
        size_t scount;
        char** stack; 
        size_t icount;
        XS_instruction** instructions;
        XS_environment* environment;
    } XS_store;

    EXPORT XS_store* XS_store_new();
    EXPORT void XS_store_emit_line(XS_store* store, char* origin, size_t line, size_t colm);
    EXPORT void XS_store_push(XS_store* store, XS_instruction* instruction);
#endif