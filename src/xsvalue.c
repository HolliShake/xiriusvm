#include "xsvalue.h"

EXPORT XS_value *XS_value_new_cint(const long long int value) {
    XS_value* new_value = XS_malloc(sizeof(XS_value));
    assert_allocation(new_value);
    new_value->type = XS_INT;
    new_value->name = NULL;
    new_value->argc = 0;
    new_value->is_async = false;
    new_value->value.int_value = value;
    return new_value;
}

EXPORT XS_value *XS_value_new_cfloat(const double value) {
    XS_value* new_value = XS_malloc(sizeof(XS_value));
    assert_allocation(new_value);
    new_value->type = XS_FLOAT;
    new_value->name = NULL;
    new_value->argc = 0;
    new_value->is_async = false;
    new_value->value.float_value = value;
    return new_value;
}

EXPORT XS_value *XS_value_new_cstring(const char *value) {
    XS_value* new_value = XS_malloc(sizeof(XS_value));
    assert_allocation(new_value);
    new_value->type = XS_STRING;
    new_value->name = NULL;
    new_value->argc = 0;
    new_value->is_async = false;
    new_value->value.string_value = str__new(value);
    return new_value;
}

EXPORT XS_value *XS_value_new_cbool(const bool value) {
    XS_value* new_value = XS_malloc(sizeof(XS_value));
    assert_allocation(new_value);
    new_value->type = XS_BOOL;
    new_value->name = NULL;
    new_value->argc = 0;
    new_value->is_async = false;
    new_value->value.bool_value = value;
    return new_value;
}

EXPORT XS_value *XS_value_new_cnull() {
    XS_value* new_value = XS_malloc(sizeof(XS_value));
    assert_allocation(new_value);
    new_value->type = XS_NULL;
    new_value->name = NULL;
    new_value->argc = 0;
    new_value->is_async = false;
    new_value->value.object = NULL;
    return new_value;
}

EXPORT char* XS_value_to_string(XS_value* value) {
    switch (value->type) {
        case XS_INT:
            return str__format("%lld", value->value.int_value);
        case XS_FLOAT:
            return str__format("%lf", value->value.float_value);
        case XS_STRING:
            return str__format("%s", value->value.string_value);
        case XS_BOOL:
            return str__format("%s", (value->value.bool_value) ? "true" : "false");
        case XS_NULL:
            return str__new("null");
        default:
            return str__new("[Not Implemented]");
    }
}

EXPORT XS_value* XS_value_new_cfunction(cfunction_t cfunction, const char* name, int argc) {
    XS_value* new_value = XS_malloc(sizeof(XS_value));
    assert_allocation(new_value);
    new_value->type = XS_NATIVE_FUNCTION;
    new_value->name = str__new(name);
    new_value->argc = argc;
    new_value->is_async = false;
    new_value->value.object = cfunction;
    return new_value;
}

#pragma region CHECKER
    EXPORT bool XS_value_is_int(XS_value *value) {
        return value->type == XS_INT;
    }

    EXPORT bool XS_value_is_float(XS_value *value) {
        return value->type == XS_FLOAT;
    }

    EXPORT bool XS_value_is_number(XS_value *value) {
        return XS_value_is_int(value) || XS_value_is_float(value);
    }

    EXPORT bool XS_value_is_string(XS_value *value) {
        return value->type == XS_STRING;
    }

    EXPORT bool XS_value_is_bool(XS_value *value) {
        return value->type == XS_BOOL;
    }

    EXPORT bool XS_value_is_null(XS_value *value) {
        return value->type == XS_NULL;
    }

    EXPORT bool XS_value_is_satisfiable(XS_value* value) {
        if (XS_value_is_int(value)) {
            return (value->value.int_value != 0);
        } else if (XS_value_is_float(value)) {
            return (value->value.float_value != 0.0);
        } else if (XS_value_is_string(value)) {
            return (strlen(value->value.string_value) > 0);
        } else if (XS_value_is_bool(value)) {
            return (value->value.bool_value == true);
        } else if (XS_value_is_null(value)) {
            return false;
        } else {
            printf("%s::%s[%d]:warning: [Not Implemented]!!!\n", __FILE__, __func__, __LINE__);
            return false;
        }
    }

    EXPORT bool XS_value_is_native_function(XS_value* value) {
        return value->type == XS_NATIVE_FUNCTION;
    }
#pragma endregion CHECKER