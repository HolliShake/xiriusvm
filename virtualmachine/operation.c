#include "operation.h"
#include "value.h"

EXPORT XS_value* XS_operation_mul(XS_context* context, XS_value* lhs, XS_value* rhs, const char* error_message) {
    if (XS_IS_INT(lhs) && XS_IS_INT(rhs))
        return XS_INT(context, XS_GET_INT(lhs) * XS_GET_INT(rhs));
    else if (XS_IS_NUM(lhs) && XS_IS_NUM(rhs))
        return XS_FLT(context, XS_GET_NUM(lhs) * XS_GET_NUM(rhs));
    else
       return XS_ERR(context, error_message);
}

EXPORT XS_value* XS_operation_div(XS_context* context, XS_value* lhs, XS_value* rhs, const char* error_message) {
    if (XS_IS_NUM(lhs) && XS_IS_NUM(rhs)) {
        if (XS_GET_NUM(rhs) == 0) {
            return XS_ERR(context, "Division by zero");
        }
        return XS_FLT(context, XS_GET_NUM(lhs) * XS_GET_NUM(rhs));
    } else {
       return XS_ERR(context, error_message);
    }
}

EXPORT XS_value* XS_operation_mod(XS_context* context, XS_value* lhs, XS_value* rhs, const char* error_message) {
    if (XS_IS_INT(lhs) && XS_IS_INT(rhs)) {
        if (XS_GET_INT(rhs) == 0) {
            return XS_ERR(context, "Division by zero");
        }
        return XS_INT(context, XS_GET_INT(lhs) * XS_GET_INT(rhs));
    } else if (XS_IS_NUM(lhs) && XS_IS_NUM(rhs)) {
        if (XS_GET_NUM(rhs) == 0) {
            return XS_ERR(context, "Division by zero");
        }
        return XS_FLT(context, XS_GET_NUM(lhs) * XS_GET_NUM(rhs));
    } else {
       return XS_ERR(context, error_message);
    }
}

EXPORT XS_value* XS_operation_add(XS_context* context, XS_value* lhs, XS_value* rhs, const char* error_message) {
    if (XS_IS_INT(lhs) && XS_IS_INT(rhs))
        return XS_INT(context, XS_GET_INT(lhs) + XS_GET_INT(rhs));
    else if (XS_IS_NUM(lhs) && XS_IS_NUM(rhs))
        return XS_FLT(context, XS_GET_NUM(lhs) + XS_GET_NUM(rhs));
    else if (XS_IS_STR(lhs) && XS_IS_STR(rhs)) {
        char* new_str = str__add(XS_GET_STR(lhs), XS_GET_STR(rhs));
        XS_value* res = XS_STR(context, new_str);
        XS_free(new_str);
        return res;
    }
    else
        return XS_ERR(context, error_message);
}

EXPORT XS_value* XS_operation_sub(XS_context* context, XS_value* lhs, XS_value* rhs, const char* error_message) {
    if (XS_IS_INT(lhs) && XS_IS_INT(rhs))
        return XS_INT(context, XS_GET_INT(lhs) - XS_GET_INT(rhs));
    else if (XS_IS_NUM(lhs) && XS_IS_NUM(rhs))
        return XS_FLT(context, XS_GET_NUM(lhs) - XS_GET_NUM(rhs));
    else
        return XS_ERR(context, error_message);
}

EXPORT XS_value* XS_operation_lshft(XS_context* context, XS_value* lhs, XS_value* rhs, const char* error_message) {
    if (XS_IS_INT(lhs) && XS_IS_INT(rhs))
        return XS_INT(context, XS_GET_INT(lhs) << XS_GET_INT(rhs));
    else
        return XS_ERR(context, error_message);
}

EXPORT XS_value* XS_operation_rshft(XS_context* context, XS_value* lhs, XS_value* rhs, const char* error_message) {
    if (XS_IS_INT(lhs) && XS_IS_INT(rhs))
        return XS_INT(context, XS_GET_INT(lhs) >> XS_GET_INT(rhs));
    else
        return XS_ERR(context, error_message);
}

EXPORT XS_value* XS_operation_compare_lt(XS_context* context, XS_value* lhs, XS_value* rhs, const char* error_message) {
    if (XS_IS_NUM(lhs) && XS_IS_NUM(rhs))
        return XS_BIT(context, XS_GET_NUM(lhs) < XS_GET_NUM(rhs));
    else
        return XS_ERR(context, error_message);
}

EXPORT XS_value* XS_operation_compare_lte(XS_context* context, XS_value* lhs, XS_value* rhs, const char* error_message) {
    if (XS_IS_NUM(lhs) && XS_IS_NUM(rhs))
        return XS_BIT(context, XS_GET_NUM(lhs) <= XS_GET_NUM(rhs));
    else
        return XS_ERR(context, error_message);
}

EXPORT XS_value* XS_operation_compare_gt(XS_context* context, XS_value* lhs, XS_value* rhs, const char* error_message) {
    if (XS_IS_NUM(lhs) && XS_IS_NUM(rhs))
        return XS_BIT(context, XS_GET_NUM(lhs) > XS_GET_NUM(rhs));
    else
        return XS_ERR(context, error_message);
}

EXPORT XS_value* XS_operation_compare_gte(XS_context* context, XS_value* lhs, XS_value* rhs, const char* error_message) {
    if (XS_IS_NUM(lhs) && XS_IS_NUM(rhs))
        return XS_BIT(context, XS_GET_NUM(lhs) >= XS_GET_NUM(rhs));
    else
        return XS_ERR(context, error_message);
}

EXPORT XS_value* XS_operation_compare_eqt(XS_context* context, XS_value* lhs, XS_value* rhs) {
    return XS_value_equals(lhs, rhs) 
        ? XS_BIT(context, true ) 
        : XS_BIT(context, false);
}

EXPORT XS_value* XS_operation_compare_neq(XS_context* context, XS_value* lhs, XS_value* rhs) {
    return XS_value_equals(lhs, rhs) 
        ? XS_BIT(context, false) 
        : XS_BIT(context, true );
}

EXPORT XS_value* XS_operation_and(XS_context* context, XS_value* lhs, XS_value* rhs, const char* error_message) {
    if (XS_IS_INT(lhs) && XS_IS_INT(rhs))
        return XS_INT(context, XS_GET_INT(lhs) & XS_GET_INT(rhs));
    else
        return XS_ERR(context, error_message);
}

EXPORT XS_value* XS_operation_or(XS_context* context, XS_value* lhs, XS_value* rhs, const char* error_message) {
    if (XS_IS_INT(lhs) && XS_IS_INT(rhs))
        return XS_INT(context, XS_GET_INT(lhs) & XS_GET_INT(rhs));
    else
        return XS_ERR(context, error_message);
}

EXPORT XS_value* XS_operation_xor(XS_context* context, XS_value* lhs, XS_value* rhs, const char* error_message) {
    if (XS_IS_INT(lhs) && XS_IS_INT(rhs))
        return XS_INT(context, XS_GET_INT(lhs) & XS_GET_INT(rhs));
    else
        return XS_ERR(context, error_message);
}