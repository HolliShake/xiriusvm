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

bool str__equals(const char* str0, const char* str1) {
    if (str0 == NULL || str1 == NULL) {
        return false;
    }
    return strcmp(str0, str1) == 0;
}

int64_t hash_double(double d) {
    // Use type punning to interpret the bits of the double as an integer
    union {
        double d;
        int64_t i;
    } u;
    
    u.d = d;

    // Hash the integer representation of the double
    // A simple example: combine the bits using bit shifts and XOR
    uint64_t hash = u.i;
    hash ^= (hash >> 32);
    hash *= 0xd6e8feb86659fd93LL;
    hash ^= (hash >> 32);
    hash *= 0xa5a5a5a5a5a5a5a5LL;
    hash ^= (hash >> 32);
    return hash;
}

int64_t hash_string(const char* str) {
    int64_t hash = 0;
    for (size_t i = 0; i < strlen(str); i++) {
        hash = (hash * 31) + str[i];
    }
    return hash;
}