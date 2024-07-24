#include "src/xirius.h"

XS_value* println(XS_context* context, XS_value** argv, int argc) {
    printf("> ");
    for (int i = 0; i < argc; i++) {
        XS_value* arg = argv[i];
        printf("%s", XS_value_to_const_string(arg));

        if (i < argc - 1) {
            printf(" ");
        }
    }
    printf("\n");
    return XS_value_new_nil(context);
}

int main() {
    XS_value* println_fn = XS_value_new_cfunction(println, false, "println", 2);

    XS_runtime* runtime = XS_runtime_new();
    XS_context* context = XS_context_new(runtime);

    store_t* store = store_new();
    
    const int a = 2, b = 3, c = 100;
    opcode_push_const(store, XS_value_new_int(context, a));
    opcode_push_const(store, XS_value_new_int(context, b));
    opcode_binary_add(store);
    opcode_push_const(store, XS_value_new_int(context, c));
    opcode_binary_mul(store);
    opcode_push_const(store, XS_value_new_int(context, a));
    opcode_binary_add(store);
    opcode_push_const(store, XS_value_new_flt(context, 10.4));
    opcode_binary_mul(store);
    opcode_pop_top(store);
    opcode_push_const(store, XS_value_new_str(context, "Hello"));
    opcode_push_const(store, XS_value_new_str(context, ", "));
    opcode_binary_add(store);
    opcode_push_const(store, XS_value_new_str(context, "World!"));
    opcode_binary_add(store);
    opcode_pop_top(store);

    /*
        println("Value:", { "A": 65, "B": 66, "C": 66.102 });
    */
    opcode_push_const(store, XS_value_new_str(context, "A"));
    opcode_push_const(store, XS_value_new_int(context, 65));
    opcode_push_const(store, XS_value_new_str(context, "B"));
    opcode_push_const(store, XS_value_new_int(context, 66));
    opcode_push_const(store, XS_value_new_str(context, "C"));
    opcode_push_const(store, XS_value_new_flt(context, 66.102));
    opcode_make_object(store, 3);
    opcode_push_const(store, XS_value_new_str(context, "Value:"));
    opcode_push_const(store, println_fn);
    opcode_call(store, 2);

    /*
        { "A": 65, "B": 66, "C": 66.102 }["c"];
    */
    opcode_push_const(store, XS_value_new_str(context, "C"));
    opcode_push_const(store, XS_value_new_str(context, "A"));
    opcode_push_const(store, XS_value_new_int(context, 65));
    opcode_push_const(store, XS_value_new_str(context, "B"));
    opcode_push_const(store, XS_value_new_int(context, 66));
    opcode_push_const(store, XS_value_new_str(context, "C"));
    opcode_push_const(store, XS_value_new_flt(context, 66.102));
    opcode_make_object(store, 3);
    opcode_pop_top(store);
    opcode_push_const(store, XS_value_new_str(context, "C"));
    opcode_get_attribute(store);
    opcode_push_const(store, XS_value_new_str(context, "LOADED:"));
    opcode_push_const(store, println_fn);
    opcode_call(store, 2);
    opcode_pop_top(store);

    /*
        if (3 > 2) {
            "Mother" + "FOCCER";
        } else {
            30 + 22;
        }
    */

    // if (3 > 2)
    opcode_push_const(store, XS_value_new_int(context, 3));
    opcode_push_const(store, XS_value_new_int(context, 2));
    opcode_compare_greater(store);
    XS_instruction* jump1 =
    opcode_pop_jump_if_false(store, 0);
        opcode_push_const(store, XS_value_new_str(context, "Nah"));
        opcode_push_const(store, XS_value_new_str(context, "Holli"));
        opcode_push_const(store, XS_value_new_str(context, "Shake!"));
        opcode_binary_add(store);
        opcode_push_const(store, println_fn);
        opcode_call(store, 2);
        opcode_pop_top(store);
        XS_instruction* jump2 =
        opcode_jump_forward(store, 0);
    // else 
    opcode_jump_to_current_offset(store, jump1);
        opcode_push_const(store, XS_value_new_int(context, 30));
        opcode_push_const(store, XS_value_new_int(context, 22));
        opcode_binary_add(store);
        opcode_push_const(store, XS_value_new_int(context, 22));
        opcode_push_const(store, println_fn);
        opcode_call(store, 2);
        opcode_pop_top(store);
    opcode_jump_to_current_offset(store, jump2);

    XS_context_free(context);
    XS_runtime_execute(context, store);
    XS_runtime_free(runtime);

    printf("DONE!\n");
    return 0;
}