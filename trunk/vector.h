/*
 * Dynamic Allocated Array-Based Vector C Implementation
 * For The Teco Project
 * Copyright (C) 2010 milkyjing <milkyjing@gmail.com>
 * Auguest 11th, 2010
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * milkyjing
 *
 */

#ifndef VECTOR_H
#define VECTOR_H
#pragma once

#include <memory.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus */

typedef void (*CleanFunc)(void *);

/* Note that if element of vector is a pointer, then the copy including vector_assign(), vector_retrieve(), vector_push_back() and vector_copy() are all low copy. */
typedef struct _vector {
    unsigned char *buffer;
    unsigned long elementSize;
    unsigned long capacity;
    unsigned long count;
    CleanFunc clean_buf;
} Vector;

extern void vector_init(Vector *v, unsigned long elementSize, unsigned long elements, CleanFunc clean_buf) {
    Vector vector;
    vector.elementSize = elementSize;
    vector.capacity = elements;
    vector.count = elements;
    vector.clean_buf = clean_buf;
    vector.buffer = (unsigned char *)malloc(vector.capacity * vector.elementSize);
    *v = vector;
}

extern unsigned long vector_get_size(const Vector *v) {
    return v->count;
}

extern int vector_assign(Vector *v, unsigned long i, const void *element) {
    if (i >= v->count) return -1;
    memcpy(v->buffer + i * v->elementSize, (const unsigned char *)element, v->elementSize);
    return 0;
}

extern int vector_retrieve(const Vector *v, unsigned long i, void *element) {
    if (i >= v->count) return -1;
    memcpy((unsigned char *)element, v->buffer + i * v->elementSize, v->elementSize);
    return 0;
}

extern void vector_push_back(Vector *v, const void *element) {
    if (v->count == v->capacity) {
        v->capacity += v->capacity / 2 + 1;
        v->buffer = (unsigned char *)realloc(v->buffer, v->capacity * v->elementSize);
    }
    memcpy(v->buffer + v->count * v->elementSize, (const unsigned char *)element, v->elementSize);
    v->count ++;
}

extern int vector_pop_back(Vector *v) {
    if (0 == v->count) return -1;
    v->count --;
    return 0;
}

extern void vector_clear(Vector *v) {
    v->clean_buf(v);
    v->buffer = NULL;
    v->capacity = 0;
    v->count = 0;
    /* v->elementSize should stay unchanged */
}

extern const void *vector_get_buf(const Vector *v) {
    return (const void *)v->buffer;
}

extern void vector_copy(Vector *dst, const Vector *src) {
    dst->elementSize = src->elementSize;
    dst->capacity = src->capacity;
    dst->count = src->count;
    dst->clean_buf = src->clean_buf;
    dst->buffer = (unsigned char *)malloc(src->count * src->elementSize);
    memcpy(dst->buffer, src->buffer, src->count * src->elementSize);
}

extern int vector_compare(const Vector *v1, const Vector *v2, unsigned long elements) {
    return memcmp(v1->buffer, v2->buffer, elements * v1->elementSize);
}

#ifdef __cplusplus
}
#endif

#endif    /* VECTOR_H */

