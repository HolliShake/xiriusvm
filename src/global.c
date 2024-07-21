#include "global.h"

char* str__new(const char* str) {
    char* new_str = XS_malloc(strlen(str) + 1);
    assert_allocation(new_str);
    new_str[0] = '\0';
    sprintf(new_str, "%s", str);
    return new_str;
}

char* str__add(char* str0, char* str1) {
    char* new_str = XS_malloc(strlen(str0) + strlen(str1) + 1);
    assert_allocation(new_str);
    new_str[0] = '\0';
    strcat(new_str, str0);
    strcat(new_str, str1);
    return new_str;
}

char* str__format(const char* format, ...) {
    va_list args;
    va_start(args, format);
    char* buffer = (char*) XS_malloc(1024 * sizeof(char));
    assert_allocation(buffer);
    vsprintf(buffer, format, args);
    va_end(args);
    return buffer;
}