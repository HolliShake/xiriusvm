#include "runtime.h"

#define MAX_STACK_SIZE 1024
#define PUSH(x) (rt->evaluation_stack[++(rt->evaluation_stack_base)] = (x))
#define POP() (rt->evaluation_stack[(rt->evaluation_stack_base)--])
#define PEEK() (rt->evaluation_stack[rt->evaluation_stack_base])

EXPORT XS_runtime* XS_runtime_new() {
    XS_runtime* runtime = XS_malloc(sizeof(XS_runtime));
    assert_allocation(runtime);
    // Initialize evaluation stack
    runtime->evaluation_stack_base = 0;
    // Initialize error
    runtime->error = NULL;
    return runtime;
}

/**/// Binary operations 
#define OPERATION_MUL(a, b) {\
        XS_value* c = NULL;\
        if (XS_value_is_int(a) && XS_value_is_int(b))\
            c = XS_value_new_int(context, (const long long int) (a->value.int_value * b->value.int_value));\
        else if (XS_value_is_flt(a) && XS_value_is_flt(b))\
            c = XS_value_new_flt(context, (const long double) (a->value.flt_value * b->value.flt_value));\
        else if (XS_value_is_int(a) && XS_value_is_flt(b))\
            c = XS_value_new_flt(context, (const long double) (a->value.int_value * b->value.flt_value));\
        else if (XS_value_is_flt(a) && XS_value_is_int(b))\
            c = XS_value_new_flt(context, (const long double) (a->value.flt_value * b->value.int_value));\
        else\
            c = NULL;\
        PUSH(c);\
    }\

#define OPERATION_ADD(a, b) {\
        XS_value* c = NULL;\
        if (XS_value_is_int(a) && XS_value_is_int(b))\
            c = XS_value_new_int(context, (const long long int) (a->value.int_value + b->value.int_value));\
        else if (XS_value_is_flt(a) && XS_value_is_flt(b))\
            c = XS_value_new_flt(context, (const long double) (a->value.flt_value + b->value.flt_value));\
        else if (XS_value_is_int(a) && XS_value_is_flt(b))\
            c = XS_value_new_flt(context, (const long double) (a->value.int_value + b->value.flt_value));\
        else if (XS_value_is_flt(a) && XS_value_is_int(b))\
            c = XS_value_new_flt(context, (const long double) (a->value.flt_value + b->value.int_value));\
        else if (XS_value_is_str(a) && XS_value_is_str(b))\
            c = XS_value_new_str(context, str__add(a->value.str_value, b->value.str_value));\
        else\
            c = NULL;\
        PUSH(c);\
    }\

#define OPERATION_SUB(a, b) {\
        XS_value* c = NULL;\
        if (XS_value_is_int(a) && XS_value_is_int(b))\
            c = XS_value_new_int(context, (const long long int) (a->value.int_value - b->value.int_value));\
        else if (XS_value_is_flt(a) && XS_value_is_flt(b))\
            c = XS_value_new_flt(context, (const long double) (a->value.flt_value - b->value.flt_value));\
        else if (XS_value_is_int(a) && XS_value_is_flt(b))\
            c = XS_value_new_flt(context, (const long double) (a->value.int_value - b->value.flt_value));\
        else if (XS_value_is_flt(a) && XS_value_is_int(b))\
            c = XS_value_new_flt(context, (const long double) (a->value.flt_value - b->value.int_value));\
        else\
            c = NULL;\
        PUSH(c);\
    }\

#define OPERATION_LT(a, b) {\
        XS_value* c = NULL;\
        if (XS_IS_INT(a) && XS_IS_INT(b))\
            c = XS_BIT(context, (const bool) (XS_GET_INT(a) < XS_GET_INT(b)));\
        else if (XS_IS_FLT(a) && XS_IS_FLT(b))\
            c = XS_BIT(context, (const bool) (XS_GET_FLT(a) < XS_GET_FLT(b)));\
        else if (XS_IS_INT(a) && XS_IS_FLT(b))\
            c = XS_BIT(context, (const bool) (XS_GET_INT(a) < XS_GET_FLT(b)));\
        else if (XS_IS_FLT(a) && XS_IS_INT(b))\
            c = XS_BIT(context, (const bool) (XS_GET_FLT(a) < XS_GET_INT(b)));\
        else\
            c = NULL;\
        PUSH(c);\
    }\

#define OPERATION_LTE(a, b) {\
        XS_value* c = NULL;\
        if (XS_IS_INT(a) && XS_IS_INT(b))\
            c = XS_BIT(context, (const bool) (XS_GET_INT(a) <= XS_GET_INT(b)));\
        else if (XS_IS_FLT(a) && XS_IS_FLT(b))\
            c = XS_BIT(context, (const bool) (XS_GET_FLT(a) <= XS_GET_FLT(b)));\
        else if (XS_IS_INT(a) && XS_IS_FLT(b))\
            c = XS_BIT(context, (const bool) (XS_GET_INT(a) <= XS_GET_FLT(b)));\
        else if (XS_IS_FLT(a) && XS_IS_INT(b))\
            c = XS_BIT(context, (const bool) (XS_GET_FLT(a) <= XS_GET_INT(b)));\
        else\
            c = NULL;\
        PUSH(c);\
    }\

#define OPERATION_GT(a, b) {\
        XS_value* c = NULL;\
        if (XS_IS_INT(a) && XS_IS_INT(b))\
            c = XS_BIT(context, (const bool) (XS_GET_INT(a) > XS_GET_INT(b)));\
        else if (XS_IS_FLT(a) && XS_IS_FLT(b))\
            c = XS_BIT(context, (const bool) (XS_GET_FLT(a) > XS_GET_FLT(b)));\
        else if (XS_IS_INT(a) && XS_IS_FLT(b))\
            c = XS_BIT(context, (const bool) (XS_GET_INT(a) > XS_GET_FLT(b)));\
        else if (XS_IS_FLT(a) && XS_IS_INT(b))\
            c = XS_BIT(context, (const bool) (XS_GET_FLT(a) > XS_GET_INT(b)));\
        else\
            c = NULL;\
        PUSH(c);\
    }\

#define OPERATION_GTE(a, b) {\
        XS_value* c = NULL;\
        if (XS_IS_INT(a) && XS_IS_INT(b))\
            c = XS_BIT(context, (const bool) (XS_GET_INT(a) >= XS_GET_INT(b)));\
        else if (XS_IS_FLT(a) && XS_IS_FLT(b))\
            c = XS_BIT(context, (const bool) (XS_GET_FLT(a) >= XS_GET_FLT(b)));\
        else if (XS_IS_INT(a) && XS_IS_FLT(b))\
            c = XS_BIT(context, (const bool) (XS_GET_INT(a) >= XS_GET_FLT(b)));\
        else if (XS_IS_FLT(a) && XS_IS_INT(b))\
            c = XS_BIT(context, (const bool) (XS_GET_FLT(a) >= XS_GET_INT(b)));\
        else\
            c = NULL;\
        PUSH(c);\
    }\

/**/// Call native function operation
#define CALL_OR_ERROR(fn, required_argc)\
    if (fn->argc != required_argc) {\
        XS_value* error = XS_value_new_err(context, (const char*) str__format("TypeError: %s() takes exactly %d arguments (%d given)", fn->name, required_argc, fn->argc));\
        PUSH(error);\
        break;\
    }\

#define CALL_NATIVE(fn, argv, argc)\
    XS_value* ret = ((cfunction_t) fn->value.obj_value)(context, argv, argc);\
    PUSH(ret);\


/****************************/
EXPORT void XS_runtime_execute(XS_context* context, store_t* store) {
    XS_runtime* rt = XS_context_get_runtime(context);

    // Call stack
    size_t   CALL_STACKI[MAX_STACK_SIZE], call_stack_base = 0ull, pointer = 0ull;
    store_t* CALL_STACKF[MAX_STACK_SIZE];
    
    // Initialize call stack
    CALL_STACKI[call_stack_base] = 0;
    CALL_STACKF[call_stack_base] = store;

    size_t i = CALL_STACKI[call_stack_base];
    for (;;) {
        XS_instruction* instruction = CALL_STACKF[pointer]->instructions[i++];
        if (instruction == NULL) {
            break;
        }
        switch (instruction->opcode) {
            // Push constant
            case PUSH_CONST:
                PUSH(instruction->value_0);
                break;
            // Object operations
            case GET_ATTRIBUTE: {
                XS_value* obj = POP();
                XS_value* ind = POP();
                if (XS_value_is_obj(obj)) {
                    XS_value* value = object_get(obj->value.obj_value, ind);
                    PUSH(
                        ( value == NULL ) 
                        ? XS_value_new_nil(context) 
                        : value
                    );
                    break;
                } else {
                    PUSH(XS_value_new_err(context, "TypeError: 'TypeName' object is not subscriptable"));
                }
                break;
            }
            case MAKE_OBJECT: {
                XS_value* obj = XS_value_new_obj(context);
                for (size_t j = 0; j < instruction->data_0; j++) {
                    XS_value* val = POP();
                    XS_value* key = POP();
                    object_set(obj->value.obj_value, key, val);
                }
                PUSH(obj);
                break;
            }
            // Other operations
            case POP_TOP:
                POP();
                break;
            case CALL: {
                XS_value* fn = POP();
                XS_value* argv[255/**** Max argc is 255 ****/];
                if (XS_value_is_native_function(fn)) {
                    for (size_t j = 0; j < instruction->data_0; j++) {
                        argv[j] = POP();
                    }
                    /**** CHECK IF SIGNITURE MATCHED ****/ 
                    CALL_OR_ERROR(fn, instruction->data_0);

                    /**** CALL FUNCTION *****************/ 
                    CALL_NATIVE(fn, argv, instruction->data_0);
                }
                break;
            }
            // Binary operations
            case BINARY_MUL: {
                XS_value* b = POP();
                XS_value* a = POP();
                OPERATION_MUL(a, b);
                break;
            }
            case BINARY_ADD: {
                XS_value* b = POP();
                XS_value* a = POP();
                OPERATION_ADD(a, b);
                break;
            }
            case BINARY_SUB: {
                XS_value* b = POP();
                XS_value* a = POP();
                OPERATION_SUB(a, b);
                break;
            }
            case COMPARE_LESS: {
                XS_value* b = POP();
                XS_value* a = POP();
                OPERATION_LT(a, b);
                break;
            }
            case COMPARE_LESS_EQUAL: {
                XS_value* b = POP();
                XS_value* a = POP();
                OPERATION_LTE(a, b);
                break;
            }
            case COMPARE_GREATER: {
                XS_value* b = POP();
                XS_value* a = POP();
                OPERATION_GT(a, b);
                break;
            }
            case COMPARE_GREATER_EQUAL: {
                XS_value* b = POP();
                XS_value* a = POP();
                OPERATION_GTE(a, b);
                break;
            }
            // Jumps
            case JUMP_IF_FALSE_OR_POP: {
                XS_value* top = PEEK();
                if (!XS_value_is_satisfiable(top)) {
                    i = opcode_get_jump_offset(instruction);
                    break;  
                }
                POP();
                break;
            }
            case JUMP_IF_TRUE_OR_POP: {
                XS_value* top = PEEK();
                if (XS_value_is_satisfiable(top)) {
                    i = opcode_get_jump_offset(instruction);
                    break;
                }
                POP();
                break;
            }
            case JUMP_IF_NOT_ERROR: {
                XS_value* top = PEEK();
                if (!XS_value_is_err(top)) {
                    i = opcode_get_jump_offset(instruction);
                    break;
                }
                break;
            }
            case POP_JUMP_IF_FALSE: {
                XS_value* top = POP();
                if (!XS_value_is_satisfiable(top)) {
                    i = opcode_get_jump_offset(instruction);
                }
                break;
            }
            case POP_JUMP_IF_TRUE: {
                XS_value* top = POP();
                if (XS_value_is_satisfiable(top)) {
                    i = opcode_get_jump_offset(instruction);
                }
                break;
            }
            case JUMP_ABSOLUTE: {
                size_t j = i;
                i = opcode_get_jump_offset(instruction);
                if (i > j) {
                    fprintf(stderr, "%s::%s[%d]: use JUMP_FORWARD instead of JUMP_ABSOLUTE!!!\n", __FILE__, __func__, __LINE__);
                    exit(1);
                }
                break;
            }
            case JUMP_FORWARD: {
                i = opcode_get_jump_offset(instruction);
                break;
            }
            default:
                fprintf(stderr, "%s::%s[%d]: Unknown opcode!!!\n", __FILE__, __func__, __LINE__);
                exit(1);
        }
    }

    i = 0;
    for (;;) {
        XS_instruction* instruction = store->instructions[i++];
        if (instruction == NULL) break;
        XS_free(instruction);
    }
}

EXPORT void XS_runtime_execute_event_loop(XS_context* context) {
    for (;;) {

    }
}

EXPORT void XS_runtime_free(XS_runtime* runtime) {
    XS_free(runtime);
}