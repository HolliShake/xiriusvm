#include "runtime.h"
#include "context.h"
#include "environment.h"
#include "object.h"
#include "opcode.h"
#include "store.h"
#include "value.h"
#include "operation.h"

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

#define icall_stack_base call_stack_base
#define fcall_stack_base call_stack_base

EXPORT void XS_runtime_execute(XS_context* context, XS_store* store) {
    XS_runtime* rt = XS_context_get_runtime(context);

    size_t/**/CALL_STACKI[MAX_STACK_SIZE], call_stack_base = 0ull, environment_base = 0ull;
    XS_store* CALL_STACKF[MAX_STACK_SIZE];
    XS_environment* ENVIRONMENT[MAX_STACK_SIZE];
    
    // Initialize call stack
    CALL_STACKI[icall_stack_base] = 0ull;
    CALL_STACKF[fcall_stack_base] = store;
    ENVIRONMENT[environment_base] = store->environment;

    size_t i = CALL_STACKI[call_stack_base];
    XS_environment* environment = ENVIRONMENT[environment_base];
    XS_instruction* instruction;
    for (;;) {
        instruction = CALL_STACKF[call_stack_base]->instructions[i++];
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
                PUSH(
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
                XS_value* value = PEEK();
                XS_environment* current = environment;
                while (current != NULL) {
                    if (XS_environment_has(current, variable)) {
                        printf("SET_GLOBAL_PROPERTY: %s = %s \n", variable, XS_value_to_const_string(value));
                        XS_environment_set(current, variable, value);
                        break;
                    }
                    current = current->parent;
                }
                if (current == NULL) {
                    POP();
                    PUSH(XS_ERR(context, str__format("NameError: name '%s' is not defined", variable)));
                }
                break;
            }
            // Constants
            case PUSH_CONST: {
                PUSH(instruction->value_0);
                break;
            }
            case PUSH_CALLBACK: {
                XS_value* value = instruction->value_0;
                if (!XS_IS_DEFINE_FUNCTION(value)) {
                    PUSH(XS_ERR(context, "TypeError: 'TypeName' object is not callable"));
                    break;
                }
                value->store->environment->parent 
                = (value->store->environment->parent == environment) 
                ? (environment)
                : (XS_environment_copy(environment));
                PUSH(value);
                break;
            }
            // Object operations
            case SET_GLOBAL_PROPERTY: {
                const char* variable = ((const char*) instruction->str_0);
                /***********************/
                XS_value* value = PEEK();
                object_set(context->global_object->value.obj_value, XS_STR(context, variable), value);
                break;
            }
            case GET_GLOBAL_PROPERTY: {
                const char* variable = ((const char*) instruction->str_0);
                /***************/
                XS_value* value = 
                object_get(context->global_object->value.obj_value, XS_STR(context, variable));
                PUSH(
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
                    PUSH(val);
                    break;
                } else {
                    PUSH(XS_ERR(context, "TypeError: 'TypeName' object is not subscriptable"));
                }
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
            case DUP: {
                PUSH(PEEK());
                break;
            }
            case DUP2: {
                XS_value* a = (rt->evaluation_stack[rt->evaluation_stack_base - 0]);
                XS_value* b = (rt->evaluation_stack[rt->evaluation_stack_base - 1]);
                PUSH(b);
                PUSH(a);
                break;
            }
            case ROTATE: {
                XS_value* a = POP();
                XS_value* b = POP();
                PUSH(a);
                PUSH(b);
                break;
            }
            case ROTATE3: {
                // A-B-C => B-C-A
                XS_value* a = POP();
                XS_value* b = POP();
                XS_value* c = POP();
                PUSH(a);
                PUSH(c);
                PUSH(b);
                break;
            }
            case ROTATE4: {
                // A-B-C-D => B-C-D-A
                XS_value* a = POP();
                XS_value* b = POP();
                XS_value* c = POP();
                XS_value* d = POP();
                PUSH(a);
                PUSH(d);
                PUSH(c);
                PUSH(b);
                break;
            }
            case POP_TOP:
                POP();
                break;
            case CALL: {
                XS_value* fn = POP();
                XS_value* argv[255/**** Max argc is 255 ****/];
                if (XS_value_is_native_function(fn)) {
                    for (size_t j = 0; j < instruction->data_0; j++)
                        argv[j] = POP();

                    int required_args = ((!fn->variadict) 
                        ? fn->argc
                        : fn->argc - 1);

                    /**** CHECK IF SIGNITURE MATCHED ****/ 
                    if (
                        ( fn->variadict && instruction->data_0 <  required_args) || 
                        (!fn->variadict && instruction->data_0 != required_args)
                    ) {
                        XS_value* error = XS_ERR(context, (const char*) str__format("TypeError: %s() takes exactly %d arguments (%d given)", fn->name, fn->argc, instruction->data_0));
                        PUSH(error);
                        break;
                    }
                    /**** CALL FUNCTION *****************/
                    XS_value* ret = ((cfunction_t) fn->value.obj_value)(context, environment, argv, instruction->data_0);
                    PUSH(ret);
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
                        PUSH(error);
                        break;
                    }
                    /**** CALL FUNCTION *****************/
                    ENVIRONMENT[++environment_base] = fn->store->environment;
                    environment = ENVIRONMENT[environment_base];

                    CALL_STACKI[call_stack_base++] = i;
                    CALL_STACKI[ call_stack_base ] = i = 0;
                    CALL_STACKF[ call_stack_base ] = fn->store;
                } else {
                    for (size_t j = 0; j < instruction->data_0; j++)
                        POP();
                    PUSH(XS_ERR(context, "TypeError: 'TypeName' object is not callable"));
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
                PUSH(c);
                break;
            }
            case POST_INCREMENT: {
                XS_value* a = POP();
                XS_value* c = NULL;
                if (XS_IS_INT(a)) {
                    c = XS_INT(context, XS_GET_INT(a) + 1);
                    PUSH(c);
                    PUSH(a);
                    break;
                }
                else if (XS_IS_FLT(a)) {
                    c = XS_FLT(context, XS_GET_FLT(a) + 1);
                    PUSH(a);
                    PUSH(c);
                    break;
                }
                else {
                    c = XS_ERR(context, str__format("TypeError: unsupported operand type for (+=): '%s'", XS_value_to_const_string(a)));
                }
                PUSH(c);
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
                PUSH(c);
                break;
            }
            case POST_DECREMENT: {
                XS_value* a = POP();
                XS_value* c = NULL;
                if (XS_IS_INT(a)) {
                    c = XS_INT(context, XS_GET_INT(a) - 1);
                    PUSH(c);
                    PUSH(a);
                    break;
                }
                else if (XS_IS_FLT(a)) {
                    c = XS_FLT(context, XS_GET_FLT(a) - 1);
                    PUSH(a);
                    PUSH(c);
                    break;
                }
                else {
                    c = XS_ERR(context, str__format("TypeError: unsupported operand type for (-=): '%s'", XS_value_to_const_string(a)));
                }
                PUSH(c);
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
                PUSH(c);
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
                PUSH(c);
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
                PUSH(c);
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
                PUSH(c);
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
                PUSH(c);
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
                PUSH(c);
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
                PUSH(c);
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
                PUSH(c);
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
                PUSH(c);
                break;
            }
            case COMPARE_EQUAL: {
                XS_value* b = POP();
                XS_value* a = POP();
                XS_value* c = XS_operation_compare_eqt(context, a, b);
                PUSH(c);
                break;
            }
            case COMPARE_NOT_EQUAL: {
                XS_value* b = POP();
                XS_value* a = POP();
                XS_value* c = XS_operation_compare_neq(context, a, b);
                PUSH(c);
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
                PUSH(c);
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
                PUSH(c);
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
                PUSH(c);
                break;
            }
            // Control
            case RETURN: {
                XS_environment_reset(environment);
                if (call_stack_base == 0)
                    break;
                environment = ENVIRONMENT[--environment_base];
                i = CALL_STACKI[--call_stack_base];
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
                fprintf(stderr, "%s::%s[%d]: Unknown opcode (%d)!!!\n", __FILE__, __func__, __LINE__, instruction->opcode);
                exit(1);
        }
    }

    if ((context->runtime->evaluation_stack_base) != 1) {
        fprintf(stderr, "%s::%s[%d]: Stack is not empty!!!\n", __FILE__, __func__, __LINE__);
        exit(1);
    }

    if (!XS_IS_NIL(PEEK())) {
        fprintf(stderr, "%s::%s[%d]: PROGRAM DID NOT RETURN NULL!!!\n", __FILE__, __func__, __LINE__);
        exit(1);
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