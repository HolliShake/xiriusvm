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

#define OPERATION_MUL(a, b) {\
        XS_value* c = NULL;\
        if (XS_value_is_int(a) && XS_value_is_int(b)) {\
            c = XS_value_new_cint(context, (const long long int) (a->value.int_value * b->value.int_value));\
        } else if (XS_value_is_float(a) && XS_value_is_float(b)) {\
            c = XS_value_new_cfloat(context, (const long double) (a->value.float_value * b->value.float_value));\
        } else if (XS_value_is_int(a) && XS_value_is_float(b)) {\
            c = XS_value_new_cfloat(context, (const long double) (a->value.int_value * b->value.float_value));\
        } else if (XS_value_is_float(a) && XS_value_is_int(b)) {\
            c = XS_value_new_cfloat(context, (const long double) (a->value.float_value * b->value.int_value));\
        } else {\
            c = NULL;\
        }\
        PUSH(c);\
    }\

#define OPERATION_ADD(a, b) {\
        XS_value* c = NULL;\
        if (XS_value_is_int(a) && XS_value_is_int(b)) {\
            c = XS_value_new_cint(context, (const long long int) (a->value.int_value + b->value.int_value));\
        } else if (XS_value_is_float(a) && XS_value_is_float(b)) {\
            c = XS_value_new_cfloat(context, (const long double) (a->value.float_value + b->value.float_value));\
        } else if (XS_value_is_int(a) && XS_value_is_float(b)) {\
            c = XS_value_new_cfloat(context, (const long double) (a->value.int_value + b->value.float_value));\
        } else if (XS_value_is_float(a) && XS_value_is_int(b)) {\
            c = XS_value_new_cfloat(context, (const long double) (a->value.float_value + b->value.int_value));\
        } else if (XS_value_is_string(a) && XS_value_is_string(b)) {\
            c = XS_value_new_cstring(context, str__add(a->value.string_value, b->value.string_value));\
        } else {\
            c = NULL;\
        }\
        PUSH(c);\
    }\

#define OPERATION_LT(a, b) {\
        XS_value* c = NULL;\
        if (XS_value_is_int(a) && XS_value_is_int(b)) {\
            c = XS_value_new_cbool(context, (const bool) (a->value.int_value < b->value.int_value));\
        } else if (XS_value_is_float(a) && XS_value_is_float(b)) {\
            c = XS_value_new_cbool(context, (const bool) (a->value.float_value < b->value.float_value));\
        } else if (XS_value_is_int(a) && XS_value_is_float(b)) {\
            c = XS_value_new_cbool(context, (const bool) (a->value.int_value < b->value.float_value));\
        } else if (XS_value_is_float(a) && XS_value_is_int(b)) {\
            c = XS_value_new_cbool(context, (const bool) (a->value.float_value < b->value.int_value));\
        } else {\
            c = NULL;\
        }\
        PUSH(c);\
    }\

#define OPERATION_GT(a, b) {\
        XS_value* c = NULL;\
        if (XS_value_is_int(a) && XS_value_is_int(b)) {\
            c = XS_value_new_cbool(context, (const bool) (a->value.int_value > b->value.int_value));\
        } else if (XS_value_is_float(a) && XS_value_is_float(b)) {\
            c = XS_value_new_cbool(context, (const bool) (a->value.float_value > b->value.float_value));\
        } else if (XS_value_is_int(a) && XS_value_is_float(b)) {\
            c = XS_value_new_cbool(context, (const bool) (a->value.int_value > b->value.float_value));\
        } else if (XS_value_is_float(a) && XS_value_is_int(b)) {\
            c = XS_value_new_cbool(context, (const bool) (a->value.float_value > b->value.int_value));\
        } else {\
            c = NULL;\
        }\
        PUSH(c);\
    }\

#define OPERATION_GTE(a, b) {\
        XS_value* c = NULL;\
        if (XS_value_is_int(a) && XS_value_is_int(b)) {\
            c = XS_value_new_cbool(context, (const bool) (a->value.int_value >= b->value.int_value));\
        } else if (XS_value_is_float(a) && XS_value_is_float(b)) {\
            c = XS_value_new_cbool(context, (const bool) (a->value.float_value >= b->value.float_value));\
        } else if (XS_value_is_int(a) && XS_value_is_float(b)) {\
            c = XS_value_new_cbool(context, (const bool) (a->value.int_value >= b->value.float_value));\
        } else if (XS_value_is_float(a) && XS_value_is_int(b)) {\
            c = XS_value_new_cbool(context, (const bool) (a->value.float_value >= b->value.int_value));\
        } else {\
            c = NULL;\
        }\
        PUSH(c);\
    }\

#define ASSERT_ARGC(required, got) {\
        if (required != got) {\
            fprintf(stderr, "%s::%s[%d]: Expected %d arguments, got %d\n", __FILE__, __func__, __LINE__, (int) required, (int) got);\
            exit(1);\
        }\
    }\

EXPORT void XS_execute(XS_context* context, store_t* store) {
    XS_runtime* rt = context_get_runtime(context);

    // Call stack
    size_t call_stack_base = 0;
    size_t CALL_STACK_INDEXES[MAX_STACK_SIZE];
    store_t* CALL_STACK[MAX_STACK_SIZE];
    
    // Initialize call stack
    CALL_STACK_INDEXES[call_stack_base] = 0;
    CALL_STACK[call_stack_base] = store;

    size_t i = CALL_STACK_INDEXES[call_stack_base];
    for (;;) {
        XS_instruction* instruction = store->instructions[i++];
        if (instruction == NULL) {
            break;
        }
        if (rt->error != NULL) {
            // TODO: Throw error
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
                if (XS_value_is_object(obj)) {
                    XS_value* val = object_get(obj->value.object, ind);
                    PUSH(val);
                } else {
                    fprintf(stderr, "%s::%s[%d]: Not implemented yet!!!\n", __FILE__, __func__, __LINE__);
                    exit(1);
                }
                break;
            }
            case MAKE_OBJECT: {
                XS_value* obj = XS_value_new_object(context);
                for (size_t j = 0; j < instruction->data_0; j++) {
                    XS_value* val = POP();
                    XS_value* key = POP();
                    object_set(obj->value.object, key, val);
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
               
                if (XS_value_is_native_function(fn)) {
                    ASSERT_ARGC(fn->argc, instruction->data_0);
                    XS_value* argv[255];
                    for (size_t j = 0; j < instruction->data_0; j++) {
                        argv[j] = POP();
                    }
                    XS_value* ret = ((cfunction_t) fn->value.object)(context, argv, fn->argc);
                    PUSH(ret);
                } else {
                    fprintf(stderr, "%s::%s[%d]: Not implemented yet!!!\n", __FILE__, __func__, __LINE__);
                    exit(1);
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
            case COMPARE_LESS: {
                XS_value* b = POP();
                XS_value* a = POP();
                OPERATION_LT(a, b);
                break;
            }
            case COMPARE_GREATER: {
                XS_value* b = POP();
                XS_value* a = POP();
                OPERATION_GT(a, b);
                break;
            }
            // Jumps
            case JUMP_IF_FALSE_OR_POP: {
                XS_value* a = PEEK();
                if (!XS_value_is_satisfiable(a)) {
                    i = opcode_get_jump_offset(instruction);
                    break;  
                }
                POP();
                break;
            }
            case JUMP_IF_TRUE_OR_POP: {
                XS_value* a = PEEK();
                if (XS_value_is_satisfiable(a)) {
                    i = opcode_get_jump_offset(instruction);
                    break;
                }
                POP();
                break;
            }
            case POP_JUMP_IF_FALSE: {
                XS_value* a = POP();
                if (!XS_value_is_satisfiable(a)) {
                    i = opcode_get_jump_offset(instruction);
                }
                break;
            }
            case POP_JUMP_IF_TRUE: {
                XS_value* a = POP();
                if (XS_value_is_satisfiable(a)) {
                    i = opcode_get_jump_offset(instruction);
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
}

EXPORT void XS_execute_event_loop(XS_context* context) {
    for (;;) {

    }
}