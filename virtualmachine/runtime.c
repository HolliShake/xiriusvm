#include "runtime.h"
#include "context.h"
#include "environment.h"
#include "object.h"
#include "opcode.h"
#include "store.h"
#include "value.h"
#include "operation.h"

#define MAX_STACK_SIZE 1024
#define PSH(x) (rt->evaluation_stack[++(rt->evaluation_stack_base)] = (x))
#define POP() (rt->evaluation_stack[(rt->evaluation_stack_base)--])
#define TOP() (rt->evaluation_stack[rt->evaluation_stack_base])

static
XS_value* XS_runtime_finalize(XS_context* context);

EXPORT XS_runtime* XS_runtime_new() {
    XS_runtime* runtime = XS_malloc(sizeof(XS_runtime));
    assert_allocation(runtime);
    // Initialize evaluation stack
    runtime->evaluation_stack_base = 0;
    // Initialize event queue
    runtime->event_queue_base = 0;
    // Initialize finalization
    runtime->finalize = XS_runtime_finalize;
    return runtime;
}

static
XS_value* XS_runtime_finalize(XS_context* context) {
    XS_runtime* rt = XS_context_get_runtime(context);
    return POP();
}

/****************************/

#define fcall_stack_base call_stack_base

EXPORT void XS_runtime_execute(XS_context* context, XS_value* executable) {
    XS_runtime* rt = XS_context_get_runtime(context);

    size_t/********/ call_stack_base = 0ull, environment_base = 0ull;
    XS_value* /****/ CALL_STACKF[MAX_STACK_SIZE];
    XS_environment*  ENVIRONMENT[MAX_STACK_SIZE];
    
    // Initialize call stack
    CALL_STACKF[fcall_stack_base] = executable;
    ENVIRONMENT[environment_base] = executable->store->environment;

    XS_environment* environment = ENVIRONMENT[environment_base];
    XS_instruction* instruction;
    #define I (CALL_STACKF[call_stack_base]->store->pointer)
    #define J (CALL_STACKF[call_stack_base]->store->icount)
    #define PSH_FRAME(x) (CALL_STACKF[++call_stack_base] = (x))
    #define TOP_FRAME() (CALL_STACKF[call_stack_base])
    #define POP_FRAME() (call_stack_base--)
    
    for (;;) {
        instruction = CALL_STACKF[fcall_stack_base]->store->instructions[I++];
        if (instruction == NULL) {
            break;
        }
        switch (instruction->opcode) {
            // Variables
            case LOAD_NAME: {
                const char* variable = ((const char*) instruction->str_0);
                /***********************/
                XS_value* value = NULL;
                XS_environment* current = environment;
                while (current != NULL) {
                    if (XS_environment_has(current, variable)) {
                        value = XS_environment_get(current, variable);
                        break;
                    }
                    current = current->parent;
                }
                PSH(
                    ( value == NULL )
                    ? XS_ERR(context, str__format("NameError: name '%s' is not defined", variable)) 
                    : value
                );
                break;
            }
            case STORE_NAME_IMMEDIATE: {
                const char* variable = ((const char*) instruction->str_0);
                /***********************/
                XS_value* value = POP();
                XS_environment_set(environment, variable, value);
                break;
            }
            case STORE_NAME: {
                const char* variable = ((const char*) instruction->str_0);
                /***********************/ 
                XS_value* value = TOP();
                XS_environment* current = environment;
                while (current != NULL) {
                    if (XS_environment_has(current, variable)) {
                        XS_environment_set(current, variable, value);
                        break;
                    }
                    current = current->parent;
                }
                if (current == NULL) {
                    POP();
                    PSH(XS_ERR(context, str__format("NameError: name '%s' is not defined", variable)));
                }
                break;
            }
            // Constants
            case PUSH_CONST: {
                PSH(instruction->value_0);
                break;
            }
            case PUSH_CALLBACK: {
                XS_value* value = instruction->value_0;
                if (!XS_IS_DEFINE_FUNCTION(value)) {
                    PSH(XS_ERR(context, "TypeError: 'TypeName' object is not callable"));
                    break;
                }
                value->store->environment->parent 
                = (value->store->environment->parent == environment) 
                ? (environment)
                : (XS_environment_copy(environment));
                PSH(value);
                break;
            }
            // Object operations
            case SET_GLOBAL_PROPERTY: {
                const char* variable = ((const char*) instruction->str_0);
                /***********************/
                XS_value* value = TOP();
                object_set(context->global_object->value.obj_value, XS_STR(context, variable), value);
                break;
            }
            case GET_GLOBAL_PROPERTY: {
                const char* variable = ((const char*) instruction->str_0);
                /***************/
                XS_value* value = 
                object_get(context->global_object->value.obj_value, XS_STR(context, variable));
                PSH(
                    ( value == NULL ) 
                    ? XS_ERR(context, str__format("NameError: name '%s' is not defined", variable))
                    : value
                );
                break;
            }
            case SET_ATTRIBUTE: {
                XS_value* val = POP();
                XS_value* obj = POP();
                XS_value* ind = POP();
                if (XS_value_is_obj(obj)) {
                    object_set(obj->value.obj_value, ind, val);
                    PSH(val);
                    break;
                } else {
                    PSH(XS_ERR(context, "TypeError: 'TypeName' object is not subscriptable"));
                }
                break;
            }
            case GET_ATTRIBUTE: {
                XS_value* obj = POP();
                XS_value* ind = POP();
                if (XS_value_is_obj(obj)) {
                    XS_value* value = object_get(obj->value.obj_value, ind);
                    PSH(
                        ( value == NULL ) 
                        ? XS_NIL(context) 
                        : value
                    );
                    break;
                } else {
                    PSH(XS_ERR(context, "TypeError: 'TypeName' object is not subscriptable"));
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
                PSH(obj);
                break;
            }
            // Other operations
            case DUP: {
                PSH(TOP());
                break;
            }
            case DUP2: {
                XS_value* a = (rt->evaluation_stack[rt->evaluation_stack_base - 0]);
                XS_value* b = (rt->evaluation_stack[rt->evaluation_stack_base - 1]);
                PSH(b);
                PSH(a);
                break;
            }
            case ROTATE: {
                XS_value* a = POP();
                XS_value* b = POP();
                PSH(a);
                PSH(b);
                break;
            }
            case ROTATE3: {
                // A-B-C => B-C-A
                XS_value* a = POP();
                XS_value* b = POP();
                XS_value* c = POP();
                PSH(a);
                PSH(c);
                PSH(b);
                break;
            }
            case ROTATE4: {
                // A-B-C-D => B-C-D-A
                XS_value* a = POP();
                XS_value* b = POP();
                XS_value* c = POP();
                XS_value* d = POP();
                PSH(a);
                PSH(d);
                PSH(c);
                PSH(b);
                break;
            }
            case POP_TOP:
                POP();
                break;
            case CALL: {
                XS_value* fn = POP();
                XS_value* argv[255];
                
                if (XS_value_is_native_function(fn)) {
                    size_t j;
                    fn->argv[instruction->data_0] = NULL;
                    for (j = instruction->data_0; j > 0; j--)
                        argv[j - 1] = POP();

                    int required_args = ((!fn->variadict) 
                        ? fn->argc
                        : fn->argc - 1);

                    /**** CHECK IF SIGNITURE MATCHED ****/ 
                    if (
                        ( fn->variadict && instruction->data_0 <  required_args) || 
                        (!fn->variadict && instruction->data_0 != required_args)
                    ) {
                        XS_value* error = XS_ERR(context, (const char*) str__format("TypeError: %s() takes exactly %d arguments (%d given)", fn->name, fn->argc, instruction->data_0));
                        PSH(error);
                        break;
                    }
                    /**** CALL FUNCTION *****************/
                    XS_value* ret = ((cfunction_t) fn->value.obj_value)(context, environment, argv, instruction->data_0);
                    PSH(ret);
                } else if (XS_value_is_define_function(fn)) {
                    int required_args = ((!fn->variadict) 
                        ? fn->argc
                        : fn->argc - 1);
                    
                    /**** CHECK IF SIGNITURE MATCHED ****/ 
                    if (
                        ( fn->variadict && instruction->data_0 <  required_args) || 
                        (!fn->variadict && instruction->data_0 != required_args)
                    ) {
                        for (size_t j = 0; j < instruction->data_0; j++)
                            POP();
                        XS_value* error = XS_ERR(context, (const char*) str__format("TypeError: %s() takes exactly %d arguments (%d given)", fn->name, fn->argc, instruction->data_0));
                        PSH(error);
                        break;
                    }
                    /**** CALL FUNCTION *****************/
                    ENVIRONMENT[++environment_base] = fn->store->environment;
                    environment = ENVIRONMENT[environment_base];
                    PSH_FRAME(fn);
                } else {
                    for (size_t j = 0; j < instruction->data_0; j++)
                        POP();
                    PSH(XS_ERR(context, "TypeError: 'TypeName' object is not callable"));
                }
                break;
            }
            // 
            case INCREMENT: {
                XS_value* a = POP();
                XS_value* c = NULL;
                if (XS_IS_INT(a))
                    c = XS_INT(context, XS_GET_INT(a) + 1);
                else if (XS_IS_FLT(a))
                    c = XS_FLT(context, XS_GET_FLT(a) + 1);
                else
                    c = XS_ERR(context, str__format("TypeError: unsupported operand type for (+=): '%s'", XS_value_to_const_string(a)));
                PSH(c);
                break;
            }
            case POST_INCREMENT: {
                XS_value* a = POP();
                XS_value* c = NULL;
                if (XS_IS_INT(a)) {
                    c = XS_INT(context, XS_GET_INT(a) + 1);
                    PSH(c);
                    PSH(a);
                    break;
                }
                else if (XS_IS_FLT(a)) {
                    c = XS_FLT(context, XS_GET_FLT(a) + 1);
                    PSH(a);
                    PSH(c);
                    break;
                }
                else {
                    c = XS_ERR(context, str__format("TypeError: unsupported operand type for (+=): '%s'", XS_value_to_const_string(a)));
                }
                PSH(c);
                break;
            }
            case DECREMENT:{
                XS_value* a = POP();
                XS_value* c = NULL;
                if (XS_IS_INT(a))
                    c = XS_INT(context, XS_GET_INT(a) - 1);
                else if (XS_IS_FLT(a))
                    c = XS_FLT(context, XS_GET_FLT(a) - 1);
                else
                    c = XS_ERR(context, str__format("TypeError: unsupported operand type for (-=): '%s'", XS_value_to_const_string(a)));
                PSH(c);
                break;
            }
            case POST_DECREMENT: {
                XS_value* a = POP();
                XS_value* c = NULL;
                if (XS_IS_INT(a)) {
                    c = XS_INT(context, XS_GET_INT(a) - 1);
                    PSH(c);
                    PSH(a);
                    break;
                }
                else if (XS_IS_FLT(a)) {
                    c = XS_FLT(context, XS_GET_FLT(a) - 1);
                    PSH(a);
                    PSH(c);
                    break;
                }
                else {
                    c = XS_ERR(context, str__format("TypeError: unsupported operand type for (-=): '%s'", XS_value_to_const_string(a)));
                }
                PSH(c);
                break;
            }
            // Binary operations
            case BINARY_MUL: {
                XS_value* b = POP();
                XS_value* a = POP();
                XS_value* c = XS_operation_mul(
                    context, 
                    a, 
                    b, 
                    str__format("TypeError: unsupported operand type(s) for (*): '%s' and '%s'", XS_value_to_const_string(a), XS_value_to_const_string(b))
                );
                PSH(c);
                break;
            }
            case BINARY_DIV: {
                XS_value* b = POP();
                XS_value* a = POP();
                XS_value* c = XS_operation_div(
                    context, 
                    a, 
                    b, 
                    str__format("TypeError: unsupported operand type(s) for (/): '%s' and '%s'", XS_value_to_const_string(a), XS_value_to_const_string(b))
                );
                PSH(c);
                break;
            }
            case BINARY_ADD: {
                XS_value* b = POP();
                XS_value* a = POP();
                XS_value* c = XS_operation_add(
                    context, 
                    a, 
                    b, 
                    str__format("TypeError: unsupported operand type(s) for (+): '%s' and '%s'", XS_value_to_const_string(a), XS_value_to_const_string(b))
                );
                PSH(c);
                break;
            }
            case BINARY_SUB: {
                XS_value* b = POP();
                XS_value* a = POP();
                XS_value* c = XS_operation_sub(
                    context, 
                    a, 
                    b, 
                    str__format("TypeError: unsupported operand type(s) for (-): '%s' and '%s'", XS_value_to_const_string(a), XS_value_to_const_string(b))
                );
                PSH(c);
                break;
            }
            case BINARY_LSHIFT: {
                XS_value* b = POP();
                XS_value* a = POP();
                XS_value* c = XS_operation_lshft(
                    context, 
                    a, 
                    b, 
                    str__format("TypeError: unsupported operand type(s) for (<<): '%s' and '%s'", XS_value_to_const_string(a), XS_value_to_const_string(b))
                );
                PSH(c);
                break;
            }
            case BINARY_RSHIFT: {
                XS_value* b = POP();
                XS_value* a = POP();
                XS_value* c = XS_operation_rshft(
                    context, 
                    a, 
                    b, 
                    str__format("TypeError: unsupported operand type(s) for (>>): '%s' and '%s'", XS_value_to_const_string(a), XS_value_to_const_string(b))
                );
                PSH(c);
                break;
            }
            case COMPARE_LESS: {
                XS_value* b = POP();
                XS_value* a = POP();
                XS_value* c = XS_operation_compare_lt(
                    context, 
                    a, 
                    b, 
                    str__format("TypeError: unsupported operand type(s) for (<): '%s' and '%s'", XS_value_to_const_string(a), XS_value_to_const_string(b))
                );
                PSH(c);
                break;
            }
            case COMPARE_LESS_EQUAL: {
                XS_value* b = POP();
                XS_value* a = POP();
                XS_value* c = XS_operation_compare_lte(
                    context, 
                    a, 
                    b, 
                    str__format("TypeError: unsupported operand type(s) for (<=): '%s' and '%s'", XS_value_to_const_string(a), XS_value_to_const_string(b))
                );
                PSH(c);
                break;
            }
            case COMPARE_GREATER: {
                XS_value* b = POP();
                XS_value* a = POP();
                XS_value* c = XS_operation_compare_gt(
                    context, 
                    a, 
                    b, 
                    str__format("TypeError: unsupported operand type(s) for (>): '%s' and '%s'", XS_value_to_const_string(a), XS_value_to_const_string(b))
                );
                PSH(c);
                break;
            }
            case COMPARE_GREATER_EQUAL: {
                XS_value* b = POP();
                XS_value* a = POP();
                XS_value* c = XS_operation_compare_gte(
                    context, 
                    a, 
                    b, 
                    str__format("TypeError: unsupported operand type(s) for (>=): '%s' and '%s'", XS_value_to_const_string(a), XS_value_to_const_string(b))
                );
                PSH(c);
                break;
            }
            case COMPARE_EQUAL: {
                XS_value* b = POP();
                XS_value* a = POP();
                XS_value* c = XS_operation_compare_eqt(context, a, b);
                PSH(c);
                break;
            }
            case COMPARE_NOT_EQUAL: {
                XS_value* b = POP();
                XS_value* a = POP();
                XS_value* c = XS_operation_compare_neq(context, a, b);
                PSH(c);
                break;
            }
            case BINARY_AND: {
                XS_value* b = POP();
                XS_value* a = POP();
                XS_value* c = XS_operation_and(
                    context, 
                    a, 
                    b, 
                    str__format("TypeError: unsupported operand type(s) for (&): '%s' and '%s'", XS_value_to_const_string(a), XS_value_to_const_string(b))
                );
                PSH(c);
                break;
            }
            case BINARY_OR: {
                XS_value* b = POP();
                XS_value* a = POP();
                XS_value* c = XS_operation_or(
                    context, 
                    a, 
                    b, 
                    str__format("TypeError: unsupported operand type(s) for (|): '%s' and '%s'", XS_value_to_const_string(a), XS_value_to_const_string(b))
                );
                PSH(c);
                break;
            }
            case BINARY_XOR: {
                XS_value* b = POP();
                XS_value* a = POP();
                XS_value* c = XS_operation_xor(
                    context, 
                    a, 
                    b, 
                    str__format("TypeError: unsupported operand type(s) for (^): '%s' and '%s'", XS_value_to_const_string(a), XS_value_to_const_string(b))
                );
                PSH(c);
                break;
            }
            // Control
            case RETURN: {
                if (call_stack_base == 0)
                    break;
                    
                I = 0;
                XS_environment* root = TOP_FRAME()->store->environment, *current = environment;
                while (root != current) {
                    XS_environment_free(current);
                    current = ENVIRONMENT[--environment_base];
                }

                environment = ENVIRONMENT[--environment_base];
                POP_FRAME();
                break;
            }
            // Environment Block
            case INITIALIZE_BLOCK: {
                ENVIRONMENT[++environment_base] = XS_environment_new(environment);
                environment = ENVIRONMENT[environment_base];
                break;
            }
            case END_BLOCK: {
                XS_environment_free(environment);
                environment = ENVIRONMENT[--environment_base];
                break;
            }
            // Jumps
            case JUMP_IF_FALSE_OR_POP: {
                XS_value* top = TOP();
                if (!XS_value_is_satisfiable(top)) {
                    I = XS_opcode_get_jump_offset(instruction);
                    break;  
                }
                POP();
                break;
            }
            case JUMP_IF_TRUE_OR_POP: {
                XS_value* top = TOP();
                if (XS_value_is_satisfiable(top)) {
                    I = XS_opcode_get_jump_offset(instruction);
                    break;
                }
                POP();
                break;
            }
            case JUMP_IF_NOT_ERROR_OR_POP: {
                XS_value* top = TOP();
                if (!XS_value_is_err(top)) {
                    I = XS_opcode_get_jump_offset(instruction);
                    break;
                }
                POP();
                break;
            }
            case POP_JUMP_IF_FALSE: {
                XS_value* top = POP();
                if (!XS_value_is_satisfiable(top)) {
                    I = XS_opcode_get_jump_offset(instruction);
                }
                break;
            }
            case POP_JUMP_IF_TRUE: {
                XS_value* top = POP();
                if (XS_value_is_satisfiable(top)) {
                    I = XS_opcode_get_jump_offset(instruction);
                }
                break;
            }
            case JUMP_ABSOLUTE: {
                size_t j = I;
                I = XS_opcode_get_jump_offset(instruction);
                if (I > j) {
                    fprintf(stderr, "%s::%s[%d]: use JUMP_FORWARD instead of JUMP_ABSOLUTE!!!\n", __FILE__, __func__, __LINE__);
                    exit(1);
                }
                break;
            }
            case JUMP_FORWARD: {
                I = XS_opcode_get_jump_offset(instruction);
                break;
            }
            default:
                fprintf(stderr, "%s::%s[%d]: Unknown opcode (%d)!!!\n", __FILE__, __func__, __LINE__, instruction->opcode);
                exit(1);
        }
    }
    #undef I
    #undef J
    #undef POP_FRAME

    XS_value* result = rt->finalize(context);
    if (result && ((context->runtime->evaluation_stack_base) != 0)) {
        fprintf(stderr, "%s::%s[%d]: Stack is not empty %s(1 != %zu) !!!\n", __FILE__, __func__, __LINE__, executable->name, context->runtime->evaluation_stack_base);
        size_t  k = 0;
        while (k < context->runtime->evaluation_stack_base) {
            XS_value* value = context->runtime->evaluation_stack[k];
            fprintf(stderr, "%s::%s[%d]: %s\n", __FILE__, __func__, __LINE__, XS_value_to_const_string(value));
            k++;
        }
        
        exit(1);
    }

    if (!result) {
        fprintf(stderr, "%s::%s[%d]: PROGRAM DID NOT EXIT SUCCESSFULLY!!!\n", __FILE__, __func__, __LINE__);
        exit(1);
    }
}

EXPORT void XS_runtime_free(XS_runtime* runtime) {
    XS_free(runtime);
}