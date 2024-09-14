#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct templated_vec_header {
    uint64_t length;
    uint64_t cap;
};

#define __define_tiny_templated_vec(type) \
    type *type##_vec_new(size_t cap); \
    void type##_vec_free(type** vec); \
    struct templated_vec_header* type##_vec_get_header(type** vec); \
    uint64_t type##_vec_length(type** vec); \
    type* type##_vec_at(type** vec, size_t index); \
    uint64_t type##_vec_capacity(type** vec); \
    void type##_vec_push(type** vec, type value); \
    type type##_vec_pop(type** vec); \
    void type##_vec_clear(type** vec); \
    void type##_vec_shrink(type** vec);
#define __define_tiny_templated_vec_impl(type) \
    type *type##_vec_new(size_t cap) { \
        type* vec = malloc(sizeof(type) * cap + sizeof(struct templated_vec_header)); \
        struct templated_vec_header* header = (struct templated_vec_header*)(vec); \
        header->length = 0; \
        header->cap = cap; \
        uint64_t header_ptr = (uint64_t)header; \
        header_ptr += sizeof(struct templated_vec_header); \
        vec = (type*)header_ptr;\
        return vec; \
    } \
    void type##_vec_free(type** vec) { \
        struct templated_vec_header* header = (struct templated_vec_header*)(((uint64_t)(*vec)) - sizeof(struct templated_vec_header)); \
        free((void*)header);\
        *vec = NULL; \
    } \
    struct templated_vec_header* type##_vec_get_header(type** vec) { \
        return (struct templated_vec_header*)(((uint64_t)(*vec)) - sizeof(struct templated_vec_header)); \
    } \
    uint64_t type##_vec_length(type** vec) { \
        struct templated_vec_header* header = type##_vec_get_header(vec); \
        return header->length; \
    } \
    type* type##_vec_at(type** vec, size_t index) { \
        if (index >= type##_vec_length(vec)) { \
            return NULL; \
        } \
        return &(*vec)[index]; \
    } \
    uint64_t type##_vec_capacity(type** vec) { \
        struct templated_vec_header* header = type##_vec_get_header(vec); \
        return header->cap; \
    } \
    void type##_vec_push(type** vec, type value) { \
        if (type##_vec_length(vec) >= type##_vec_capacity(vec)) { \
            type* new_vec = type##_vec_new(type##_vec_capacity(vec) * 2); \
            memcpy(new_vec, *vec, type##_vec_length(vec) * sizeof(type)); \
            struct templated_vec_header* header = type##_vec_get_header(&new_vec); \
            header->length = type##_vec_length(vec);\
            type##_vec_free(vec); \
            *vec = new_vec; \
        } \
        struct templated_vec_header* header = type##_vec_get_header(vec); \
        (*vec)[header->length++] = value; \
    } \
    type type##_vec_pop(type** vec) {  \
        struct templated_vec_header* header = type##_vec_get_header(vec);\
        return (*vec)[--header->length]; \
    } \
    void type##_vec_clear(type** vec) { \
        struct templated_vec_header* header = type##_vec_get_header(vec); \
        header->length = 0; \
    } \
    void type##_vec_shrink(type** vec) { \
        uint64_t length = type##_vec_length(vec); \
        if (length < type##_vec_capacity(vec)) {  \
            type* new_vec = type##_vec_new(length);\
            memcpy(new_vec, *vec, length * sizeof(type));\
            type##_vec_free(vec);\
            *vec = new_vec;\
        }\
    }
