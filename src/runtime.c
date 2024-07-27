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

/****************************/
EXPORT void XS_runtime_execute(XS_context* context, XS_store* store) {
    XS_runtime* rt = XS_context_get_runtime(context);

    // Call stack
    size_t/**/CALL_STACKI[MAX_STACK_SIZE], call_stack_base = 0ull;
    XS_store* CALL_STACKF[MAX_STACK_SIZE];
    
    // Initialize call stack
    CALL_STACKI[call_stack_base] = 0ull;
    CALL_STACKF[call_stack_base] = store;

    size_t i = CALL_STACKI[call_stack_base];
    for (;;) {
        XS_instruction* instruction = CALL_STACKF[call_stack_base]->instructions[i++];
        if (instruction == NULL) {
            break;
        }
        switch (instruction->opcode) {
            // Variables
            case LOAD_NAME: {
                // CALL_STACKF[pointer]->
                XS_value* value = NULL;
                PUSH(
                    ( value == NULL )
                    ? XS_NIL(context) 
                    : value
                );
                break;
            }
            // Constants
            case PUSH_CONST:
                PUSH(instruction->value_0);
                break;
            // Object operations
            case SET_GLOBAL_PROPERTY: {
                XS_value* value = PEEK();
                object_set(context->global_object->value.obj_value, XS_STR(context, instruction->str_0), value);
                break;
            }
            case GET_GLOBAL_PROPERTY: {
                XS_value* value = 
                object_get(context->global_object->value.obj_value, XS_STR(context, instruction->str_0));
                PUSH(
                    ( value == NULL ) 
                    ? XS_NIL(context) 
                    : value
                );
                break;
            }
            case GET_ATTRIBUTE: {
                XS_value* obj = POP();
                XS_value* ind = POP();
                if (XS_value_is_obj(obj)) {
                    XS_value* value = object_get(obj->value.obj_value, ind);
                    PUSH(
                        ( value == NULL ) 
                        ? XS_NIL(context) 
                        : value
                    );
                    break;
                } else {
                    PUSH(XS_ERR(context, "TypeError: 'TypeName' object is not subscriptable"));
                }
                break;
            }
            case MAKE_OBJECT: {
                XS_value* obj = XS_OBJ(context);
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
                    if (fn->argc != instruction->data_0) {
                        XS_value* error = XS_ERR(context, (const char*) str__format("TypeError: %s() takes exactly %d arguments (%d given)", fn->name, fn->argc, instruction->data_0));
                        PUSH(error);
                        break;
                    }
                    /**** CALL FUNCTION *****************/
                    XS_value* ret = ((cfunction_t) fn->value.obj_value)(context, argv, instruction->data_0);
                    PUSH(ret);
                }
                break;
            }
            // Binary operations
            case BINARY_MUL: {
                XS_value* b = POP();
                XS_value* a = POP();
                XS_value* c = NULL;
                if (XS_IS_INT(a) && XS_IS_INT(b))
                    c = XS_INT(context, XS_GET_INT(a) * XS_GET_INT(b));
                else if (XS_IS_FLT(a) && XS_IS_FLT(b))
                    c = XS_FLT(context, XS_GET_FLT(a) * XS_GET_FLT(b));
                else if (XS_IS_NUM(a) && XS_IS_NUM(b))
                    c = XS_FLT(context, XS_GET_NUM(a) * XS_GET_NUM(b));
                else
                    c = NULL;
                PUSH(c);
                break;
            }
            case BINARY_ADD: {
                XS_value* b = POP();
                XS_value* a = POP();
                XS_value* c = NULL;
                if (XS_IS_INT(a) && XS_IS_INT(b))
                    c = XS_INT(context, XS_GET_INT(a) + XS_GET_INT(b));
                else if (XS_IS_FLT(a) && XS_IS_FLT(b))
                    c = XS_FLT(context, XS_GET_FLT(a) + XS_GET_FLT(b));
                else if (XS_IS_NUM(a) && XS_IS_NUM(b))
                    c = XS_FLT(context, XS_GET_NUM(a) + XS_GET_NUM(b));
                else if (XS_IS_STR(a) && XS_IS_STR(b))
                    c = XS_STR(context, str__add(XS_GET_STR(a), XS_GET_STR(b)));
                else
                    c = NULL;
                PUSH(c);
                break;
            }
            case BINARY_SUB: {
                XS_value* b = POP();
                XS_value* a = POP();
                XS_value* c = NULL;
                if (XS_IS_INT(a) && XS_IS_INT(b))
                    c = XS_INT(context, XS_GET_INT(a) - XS_GET_INT(b));
                else if (XS_IS_FLT(a) && XS_IS_FLT(b))
                    c = XS_FLT(context, XS_GET_FLT(a) - XS_GET_FLT(b));
                else if (XS_IS_NUM(a) && XS_IS_NUM(b))
                    c = XS_FLT(context, XS_GET_NUM(a) - XS_GET_NUM(b));
                else
                    c = NULL;
                PUSH(c);
                break;
            }
            case BINARY_LSHIFT: {
                XS_value* b = POP();
                XS_value* a = POP();
                XS_value* c = NULL;
                if (XS_IS_INT(a) && XS_IS_INT(b))
                    c = XS_INT(context, XS_GET_INT(a) << XS_GET_INT(b));
                else
                    c = NULL;
                PUSH(c);
                break;
            }
            case BINARY_RSHIFT: {
                XS_value* b = POP();
                XS_value* a = POP();
                XS_value* c = NULL;
                if (XS_IS_INT(a) && XS_IS_INT(b))
                    c = XS_INT(context, XS_GET_INT(a) >> XS_GET_INT(b));
                else
                    c = NULL;
                PUSH(c);
                break;
            }
            case COMPARE_LESS: {
                XS_value* b = POP();
                XS_value* a = POP();
                XS_value* c = NULL;
                if (XS_IS_INT(a) && XS_IS_INT(b))
                    c = XS_BIT(context, XS_GET_INT(a) < XS_GET_INT(b));
                else if (XS_IS_FLT(a) && XS_IS_FLT(b))
                    c = XS_BIT(context, XS_GET_FLT(a) < XS_GET_FLT(b));
                else if (XS_IS_NUM(a) && XS_IS_NUM(b))
                    c = XS_BIT(context, XS_GET_NUM(a) < XS_GET_NUM(b));
                else
                    c = NULL;
                PUSH(c);
                break;
            }
            case COMPARE_LESS_EQUAL: {
                XS_value* b = POP();
                XS_value* a = POP();
                XS_value* c = NULL;
                if (XS_IS_INT(a) && XS_IS_INT(b))
                    c = XS_BIT(context, XS_GET_INT(a) <= XS_GET_INT(b));
                else if (XS_IS_FLT(a) && XS_IS_FLT(b))
                    c = XS_BIT(context, XS_GET_FLT(a) <= XS_GET_FLT(b));
                else if (XS_IS_NUM(a) && XS_IS_NUM(b))
                    c = XS_BIT(context, XS_GET_NUM(a) <= XS_GET_NUM(b));
                else
                    c = NULL;
                PUSH(c);
                break;
            }
            case COMPARE_GREATER: {
                XS_value* b = POP();
                XS_value* a = POP();
                XS_value* c = NULL;
                if (XS_IS_INT(a) && XS_IS_INT(b))
                    c = XS_BIT(context, XS_GET_INT(a) > XS_GET_INT(b));
                else if (XS_IS_FLT(a) && XS_IS_FLT(b))
                    c = XS_BIT(context, XS_GET_FLT(a) > XS_GET_FLT(b));
                else if (XS_IS_NUM(a) && XS_IS_NUM(b))
                    c = XS_BIT(context, XS_GET_NUM(a) > XS_GET_NUM(b));
                else
                    c = NULL;
                PUSH(c);
                break;
            }
            case COMPARE_GREATER_EQUAL: {
                XS_value* b = POP();
                XS_value* a = POP();
                XS_value* c = NULL;
                if (XS_IS_INT(a) && XS_IS_INT(b))
                    c = XS_BIT(context, XS_GET_INT(a) >= XS_GET_INT(b));
                else if (XS_IS_FLT(a) && XS_IS_FLT(b))
                    c = XS_BIT(context, XS_GET_FLT(a) >= XS_GET_FLT(b));
                else if (XS_IS_NUM(a) && XS_IS_NUM(b))
                    c = XS_BIT(context, XS_GET_NUM(a) >= XS_GET_NUM(b));
                else
                    c = NULL;
                PUSH(c);
                break;
            }
            case COMPARE_EQUAL: {
                XS_value* b = POP();
                XS_value* a = POP();
                XS_value* c = NULL;
                c = XS_BIT(context, (const bool) XS_value_equals(a, b));
                PUSH(c);
                break;
            }
            case COMPARE_NOT_EQUAL: {
                XS_value* b = POP();
                XS_value* a = POP();
                XS_value* c = NULL;
                c = XS_BIT(context, (const bool) !XS_value_equals(a, b));
                PUSH(c);
                break;
            }
            case BINARY_AND: {
                XS_value* b = POP();
                XS_value* a = POP();
                XS_value* c = NULL;
                if (XS_IS_INT(a) && XS_IS_INT(b))
                    c = XS_BIT(context, XS_GET_INT(a) && XS_GET_INT(b));
                else
                    c = NULL;
                PUSH(c);
                break;
            }
            case BINARY_OR: {
                XS_value* b = POP();
                XS_value* a = POP();
                XS_value* c = NULL;
                if (XS_IS_INT(a) && XS_IS_INT(b))
                    c = XS_BIT(context, XS_GET_INT(a) || XS_GET_INT(b));
                else
                    c = NULL;
                PUSH(c);
                break;
            }
            case BINARY_XOR: {
                XS_value* b = POP();
                XS_value* a = POP();
                XS_value* c = NULL;
                if (XS_IS_INT(a) && XS_IS_INT(b))
                    c = XS_BIT(context, XS_GET_INT(a) ^ XS_GET_INT(b));
                else
                    c = NULL;
                PUSH(c);
                break;
            }
            // Control
            case RETURN: {
                if (call_stack_base == 0) {
                    return;
                }
                i = CALL_STACKI[--call_stack_base];
                break;
            }
            // Jumps
            case JUMP_IF_FALSE_OR_POP: {
                XS_value* top = PEEK();
                if (!XS_value_is_satisfiable(top)) {
                    i = XS_opcode_get_jump_offset(instruction);
                    break;  
                }
                POP();
                break;
            }
            case JUMP_IF_TRUE_OR_POP: {
                XS_value* top = PEEK();
                if (XS_value_is_satisfiable(top)) {
                    i = XS_opcode_get_jump_offset(instruction);
                    break;
                }
                POP();
                break;
            }
            case JUMP_IF_NOT_ERROR_OR_POP: {
                XS_value* top = PEEK();
                if (!XS_value_is_err(top)) {
                    i = XS_opcode_get_jump_offset(instruction);
                    break;
                }
                POP();
                break;
            }
            case POP_JUMP_IF_FALSE: {
                XS_value* top = POP();
                if (!XS_value_is_satisfiable(top)) {
                    i = XS_opcode_get_jump_offset(instruction);
                }
                break;
            }
            case POP_JUMP_IF_TRUE: {
                XS_value* top = POP();
                if (XS_value_is_satisfiable(top)) {
                    i = XS_opcode_get_jump_offset(instruction);
                }
                break;
            }
            case JUMP_ABSOLUTE: {
                size_t j = i;
                i = XS_opcode_get_jump_offset(instruction);
                if (i > j) {
                    fprintf(stderr, "%s::%s[%d]: use JUMP_FORWARD instead of JUMP_ABSOLUTE!!!\n", __FILE__, __func__, __LINE__);
                    exit(1);
                }
                break;
            }
            case JUMP_FORWARD: {
                i = XS_opcode_get_jump_offset(instruction);
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
        XS_instruction_free(instruction);
    }
}

EXPORT void XS_runtime_execute_event_loop(XS_context* context) {
    for (;;) {

    }
}

EXPORT void XS_runtime_free(XS_runtime* runtime) {
    XS_free(runtime);
}