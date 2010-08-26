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

typedef void (*CopyFunc)(void *, void *, const void *);    /* vector, dst, src */
typedef void (*CleanFunc)(void *);    /* vector */

/* Note that if element of vector is a pointer, then the copy including vector_assign(), vector_retrieve(), vector_push_back() and vector_copy() are all low copy. */
typedef struct _vector {
    unsigned char *buffer;
    unsigned long elementSize;
    unsigned long capacity;
    unsigned long count;
    CopyFunc copy_element;
    CleanFunc clean_buf;
} Vector;

#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus */

extern void vector_default_copy_element(void *v, void *src, const void *dst) {
    memcpy(src, dst, ((Vector *)v)->elementSize);
}

extern void vector_init(Vector *v, unsigned long elementSize, unsigned long elements, CopyFunc copy_element, CleanFunc clean_buf) {
    Vector vector;
    vector.elementSize = elementSize;
    vector.capacity = elements;
    vector.count = elements;
    vector.copy_element = copy_element;
    vector.clean_buf = clean_buf;
    vector.buffer = (unsigned char *)malloc(vector.capacity * vector.elementSize);
    *v = vector;
}

extern unsigned long vector_get_size(const Vector *v) {
    return v->count;
}

extern int vector_assign(Vector *v, unsigned long i, const void *element) {
    if (i >= v->count) return -1;
    v->copy_element(v, v->buffer + i * v->elementSize, element);
    return 0;
}

extern int vector_retrieve(const Vector *v, unsigned long i, void *element) {
    if (i >= v->count) return -1;
    v->copy_element((Vector *)v, element, v->buffer + i * v->elementSize);
    return 0;
}

extern void vector_push_back(Vector *v, const void *element) {
    if (v->count == v->capacity) {
        v->capacity += v->capacity / 2 + 1;
        v->buffer = (unsigned char *)realloc(v->buffer, v->capacity * v->elementSize);
    }
    v->copy_element(v, v->buffer + v->count * v->elementSize, element);
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
    dst->copy_element = src->copy_element;
    dst->clean_buf = src->clean_buf;
    dst->buffer = (unsigned char *)malloc(dst->capacity * dst->elementSize);
    if (vector_default_copy_element == src->copy_element)
        memcpy(dst->buffer, src->buffer, src->count * src->elementSize);
    else {
        unsigned long i;
        for (i = 0; i < src->count; i ++)
            dst->copy_element(dst, dst->buffer + i * dst->elementSize, src->buffer + i * src->elementSize);
    }
}

extern int vector_compare(const Vector *v1, const Vector *v2, unsigned long elements) {
    return memcmp(v1->buffer, v2->buffer, elements * v1->elementSize);
}

#ifdef __cplusplus
}
#endif

#endif    /* VECTOR_H */

