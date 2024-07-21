#include "object.h"

 #ifndef XSVALUE_H
    /*virtual*/ typedef struct xirius_value_struct XS_value;
    /*virtual*/ extern const char* XS_value_to_cstring(XS_value* value);
    /*virtual*/ extern bool XS_value_equals(XS_value* a, XS_value* b);
    /*virtual*/ extern long long int XS_value_hash(XS_value* value);
#endif

EXPORT object_t* object_new() {
    object_t* new_object = XS_malloc(sizeof(object_t));
    assert_allocation(new_object);
    new_object->buckets = XS_malloc(sizeof(object_node_t*) * 16);
    assert_allocation(new_object->buckets);
    new_object->element_count = 0;
    new_object->capacity = 16;
    for (size_t i = 0; i < new_object->capacity; i++) {
        new_object->buckets[i] = NULL;
    }
    return new_object;
}

EXPORT void object_set(object_t* object, XS_value* key, XS_value* value) {
    size_t hash = XS_value_hash(key) % object->capacity;
    object_node_t* node = object->buckets[hash];
    while (node != NULL) {
        if (XS_value_equals(node->key, key)) {
            node->value = value;
            return;
        }
        node = node->next;
    }

    object_node_t* new_node = XS_malloc(sizeof(object_node_t));
    assert_allocation(new_node);
    new_node->key = key;
    new_node->value = value;
    new_node->next = object->buckets[hash];
    object->buckets[hash] = new_node;
    object->element_count++;
    if (object->element_count > object->capacity * 0.75) {
        object_resize(object, object->capacity * 2);
    }
}

EXPORT XS_value* object_get(object_t* object, XS_value* key) {
    size_t hash = XS_value_hash(key) % object->capacity;
    object_node_t* node = object->buckets[hash];
    while (node != NULL) {
        if (XS_value_equals(node->key, key)) {
            return node->value;
        }
        node = node->next;
    }
    return NULL;
}

EXPORT void object_resize(object_t* object, size_t new_capacity) {
    object_node_t** new_buckets = XS_malloc(sizeof(object_node_t*) * new_capacity);
    assert_allocation(new_buckets);
    for (size_t i = 0; i < new_capacity; i++) {
        new_buckets[i] = NULL;
    }
    for (size_t i = 0; i < object->capacity; i++) {
        object_node_t* node = object->buckets[i];
        while (node != NULL) {
            size_t hash = XS_value_hash(node->key) % new_capacity;
            object_node_t* next = node->next;
            node->next = new_buckets[hash];
            new_buckets[hash] = node;
            node = next;
        }
    }
    XS_free(object->buckets);
    object->buckets = new_buckets;
    object->capacity = new_capacity;
}

EXPORT char* object_to_string(object_t* object) {
    char* str = str__new(""), *old = NULL;
    for (size_t i = 0; i < object->capacity; i++) {
        object_node_t* node = object->buckets[i], *copy = node;
        while (node != NULL) {
            char* key_str = (char*) XS_value_to_cstring(node->key  );
            char* val_str = (char*) XS_value_to_cstring(node->value);
            char* new_str = str__format("%s: %s", key_str, val_str);
            str = str__add(old = str, new_str);
            XS_free(key_str);
            XS_free(val_str);
            XS_free(old);
            node = node->next;

            if (node != NULL) {
                str = str__add(old = str, ", ");
                XS_free(old);
            }
            XS_free(new_str);
        }

        if ((i + 1) >= object->capacity) {
            continue;
        }

        bool has_next = object->buckets[i + 1] != NULL;

        if (has_next && copy != NULL && i < object->capacity - 1) {
            str = str__add(old = str, ", ");
            XS_free(old);
        }
    }
    return str__format("{ %s }", str);
}