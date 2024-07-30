#include "global.h"

#ifndef OPERATION_H
#define OPERATION_H
    EXPORT XS_value* XS_operation_mul(XS_context* context, XS_value* lhs, XS_value* rhs, const char* error_message);
    EXPORT XS_value* XS_operation_div(XS_context* context, XS_value* lhs, XS_value* rhs, const char* error_message);
    EXPORT XS_value* XS_operation_mod(XS_context* context, XS_value* lhs, XS_value* rhs, const char* error_message);
    EXPORT XS_value* XS_operation_add(XS_context* context, XS_value* lhs, XS_value* rhs, const char* error_message);
    EXPORT XS_value* XS_operation_sub(XS_context* context, XS_value* lhs, XS_value* rhs, const char* error_message);
    EXPORT XS_value* XS_operation_lshft(XS_context* context, XS_value* lhs, XS_value* rhs, const char* error_message);
    EXPORT XS_value* XS_operation_rshft(XS_context* context, XS_value* lhs, XS_value* rhs, const char* error_message);
    EXPORT XS_value* XS_operation_compare_lt(XS_context* context, XS_value* lhs, XS_value* rhs, const char* error_message);
    EXPORT XS_value* XS_operation_compare_lte(XS_context* context, XS_value* lhs, XS_value* rhs, const char* error_message);
    EXPORT XS_value* XS_operation_compare_gt(XS_context* context, XS_value* lhs, XS_value* rhs, const char* error_message);
    EXPORT XS_value* XS_operation_compare_gte(XS_context* context, XS_value* lhs, XS_value* rhs, const char* error_message);
    EXPORT XS_value* XS_operation_compare_eqt(XS_context* context, XS_value* lhs, XS_value* rhs);
    EXPORT XS_value* XS_operation_compare_neq(XS_context* context, XS_value* lhs, XS_value* rhs);
    EXPORT XS_value* XS_operation_and(XS_context* context, XS_value* lhs, XS_value* rhs, const char* error_message);
    EXPORT XS_value* XS_operation_or(XS_context* context, XS_value* lhs, XS_value* rhs, const char* error_message);
    EXPORT XS_value* XS_operation_xor(XS_context* context, XS_value* lhs, XS_value* rhs, const char* error_message);
#endif