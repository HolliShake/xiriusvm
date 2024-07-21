#include "src/xirius.h"

XS_value* println(XS_context* ctx, XS_value** argv, int argc) {
    printf("> ");
    for (int i = 0; i < argc; i++) {
        XS_value* arg = argv[i];
        printf("%s", XS_value_to_string(arg));

        if (i < argc - 1) {
            printf(" ");
        }
    }
    printf("\n");
    return XS_value_new_cnull();
}

int main() {
    XS_value* println_fn = XS_value_new_cfunction(println, "println", 2);

    XS_runtime* runtime = XS_runtime_new();
    XS_context* context = XS_context_new(runtime);

    store_t* store = store_new();
    
    const int a = 2, b = 3, c = 100;
    opcode_push_const(store, XS_value_new_cint(a));
    opcode_push_const(store, XS_value_new_cint(b));
    opcode_binary_add(store);
    opcode_push_const(store, XS_value_new_cint(c));
    opcode_binary_mul(store);
    opcode_push_const(store, XS_value_new_cint(a));
    opcode_binary_add(store);
    opcode_push_const(store, XS_value_new_cfloat(10.4));
    opcode_binary_mul(store);
    opcode_pop_top(store);
    opcode_push_const(store, XS_value_new_cstring("Hello, "));
    opcode_push_const(store, XS_value_new_cstring("World!"));
    opcode_binary_add(store);
    opcode_pop_top(store);

    /*
        if (3 > 2) {
            "Mother" + "FOCCER";
        } else {
            30 + 22;
        }
    */

    // if (3 > 2)
    opcode_push_const(store, XS_value_new_cint(2));
    opcode_push_const(store, XS_value_new_cint(3));
    opcode_compare_less(store);
    XS_instruction* jump1 =
    opcode_pop_jump_if_false(store, 0);
        opcode_push_const(store, XS_value_new_cstring("Nah"));
        opcode_push_const(store, XS_value_new_cstring("Holli"));
        opcode_push_const(store, XS_value_new_cstring("Shake!"));
        opcode_binary_add(store);
        opcode_push_const(store, println_fn);
        opcode_call(store, 2);
        opcode_pop_top(store);
        XS_instruction* jump2 =
        opcode_jump_forward(store, 0);
    // else 
    opcode_set_jump_offset(jump1, opcode_get_current_jump_offset(store));
        opcode_push_const(store, XS_value_new_cint(30));
        opcode_push_const(store, XS_value_new_cint(22));
        opcode_binary_add(store);
        opcode_push_const(store, println_fn);
        opcode_call(store, 1);
        opcode_pop_top(store);
    opcode_set_jump_offset(jump2, opcode_get_current_jump_offset(store));

    XS_execute(context, store);
    printf("DONE!\n");
    return 0;
}