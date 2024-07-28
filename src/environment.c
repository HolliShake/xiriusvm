#include "environment.h"

XS_environment* XS_environment_new(XS_environment* parent) {
    XS_environment* environment = XS_malloc(sizeof(XS_environment));
    assert_allocation(environment);
    environment->parent = parent;
    environment->bucket = XS_malloc(sizeof(XS_environment_cell*) * 16);
    assert_allocation(environment->bucket);
    environment->elements = 0;
    environment->capacity = 16;
    for (size_t i = 0; i < environment->capacity; i++) {
        environment->bucket[i] = NULL;
    }
    return environment;
}

/*virtual*/ extern int64_t hash_string(const char* str);

void XS_environment_set(XS_environment* environment, const char* name, XS_value* value) {
    size_t hash = hash_string(name) % environment->capacity;
    XS_environment_cell* cell = environment->bucket[hash];
    while (cell != NULL) {
        if (str__equals(cell->name, name)) {
            cell->value = value;
            return;
        }
        cell = cell->next;
    }
    XS_environment_cell* new_cell = XS_malloc(sizeof(XS_environment_cell));
    assert_allocation(new_cell);
    new_cell->name = str__new(name);
    new_cell->value = value;
    new_cell->next = environment->bucket[hash];
    environment->bucket[hash] = new_cell;
    environment->elements++;
    if (environment->elements > environment->capacity * 0.75) {
        XS_environment_resize(environment);
    }
}

XS_value* XS_environment_get(XS_environment* environment, const char* name) {
    size_t hash = hash_string(name) % environment->capacity;
    XS_environment_cell* cell = environment->bucket[hash];
    while (cell != NULL) {
        if (str__equals(cell->name, name)) {
            return cell->value;
        }
        cell = cell->next;
    }
    if (environment->parent != NULL) {
        return XS_environment_get(environment->parent, name);
    }
    return NULL;
}

bool XS_environment_has(XS_environment* environment, const char* name) {
    size_t hash = hash_string(name) % environment->capacity;
    XS_environment_cell* cell = environment->bucket[hash];
    while (cell != NULL) {
        if (str__equals(cell->name, name)) {
            return true;
        }
        cell = cell->next;
    }
    return false;
}

void XS_environment_resize(XS_environment* environment) {
    XS_environment_cell** new_bucket = XS_malloc(sizeof(XS_environment_cell*) * environment->capacity * 2);
    assert_allocation(new_bucket);
    for (size_t i = 0; i < environment->capacity * 2; i++) {
        new_bucket[i] = NULL;
    }
    for (size_t i = 0; i < environment->capacity; i++) {
        XS_environment_cell* cell = environment->bucket[i];
        while (cell != NULL) {
            size_t hash = hash_string(cell->name) % (environment->capacity * 2);
            XS_environment_cell* next = cell->next;
            cell->next = new_bucket[hash];
            new_bucket[hash] = cell;
            cell = next;
        }
    }
    XS_free(environment->bucket);
    environment->bucket = new_bucket;
    environment->capacity *= 2;
}

void XS_environment_reset(XS_environment* environment) {
    for (size_t i = 0; i < environment->capacity; i++) {
        XS_environment_cell* cell = environment->bucket[i];
        while (cell != NULL) {
            XS_environment_cell* next = cell->next;
            XS_free((char*) cell->name);
            XS_free(cell);
            cell = next;
        }
        environment->bucket[i] = NULL;
    }
    environment->elements = 0;
}

XS_environment* XS_environment_copy(XS_environment* environment) {
    XS_environment* new_environment = XS_malloc(sizeof(XS_environment));
    assert_allocation(new_environment);
    new_environment->parent = environment->parent;
    new_environment->elements = environment->elements;
    new_environment->capacity = environment->capacity;
    new_environment->bucket = XS_malloc(sizeof(XS_environment_cell*) * new_environment->capacity);
    assert_allocation(new_environment->bucket);
    for (size_t i = 0; i < new_environment->capacity; i++) {
        new_environment->bucket[i] = NULL;
    }
    for (size_t i = 0; i < environment->capacity; i++) {
        XS_environment_cell* cell = environment->bucket[i];
        while (cell != NULL) {
            XS_environment_set(new_environment, cell->name, cell->value);
            cell = cell->next;
        }
    }
    return new_environment;
}

void XS_environment_dump(XS_environment* environment) {
    for (size_t i = 0; i < environment->capacity; i++) {
        XS_environment_cell* cell = environment->bucket[i];
        while (cell != NULL) {
            printf("%s := %s\n", cell->name, XS_value_to_const_string(cell->value));
            cell = cell->next;
        }
    }
}

void XS_environment_free(XS_environment* environment) {
    for (size_t i = 0; i < environment->capacity; i++) {
        XS_environment_cell* cell = environment->bucket[i];
        while (cell != NULL) {
            XS_environment_cell* next = cell->next;
            XS_free((char*) cell->name);
            XS_free(cell);
            cell = next;
        }
    }
    XS_free(environment->bucket);
    XS_free(environment);
}