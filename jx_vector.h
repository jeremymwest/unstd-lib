/*******************************************************************************
 * 
 * Copyright (c) 2015, Jeremy West. Distributed under the MIT license.
 *
 ******************************************************************************/
#ifndef JX_VECTOR_H
#define JX_VECTOR_H
#include "jinks.h"

jx_result jx_vector_init(jx_vector *out_self, size_t isz, int capacity,
    jx_destructor destroy);

jx_result jx_vector_clone(const jx_vector *self, jx_vector *out_self);

void jx_vector_destroy(void *vector);

/******************************************************************************/

bool jx_vector_isempty(const jx_vector *self);

int jx_vector_size(const jx_vector *self);

int jx_vector_capacity(const jx_vector *self);

jx_result jx_vector_reserve(jx_vector *self, int num);

jx_result jx_vector_shrink(jx_vector *self);

/******************************************************************************/

void* jx_vector_front(const jx_vector *self);

void* jx_vector_back(const jx_vector *self);

void* jx_vector_at(const jx_vector *self, int i);

void jx_vector_set(jx_vector *self, int i, int num, const void *items);

void* jx_vector_data(const jx_vector *self);

/******************************************************************************/

jx_result jx_vector_prepend(jx_vector *self, int num, jx_outptr out_ptr);

jx_result jx_vector_append(jx_vector *self, int num, jx_outptr out_ptr);

jx_result jx_vector_insert(jx_vector *self, int i, int num, jx_outptr out_ptr);

/******************************************************************************/

void jx_vector_remove(jx_vector *self, int i, int num);

void jx_vector_pop_back(jx_vector *self, int num);

void jx_vector_clear(jx_vector *self);

/******************************************************************************/

/* TODO: 
 *  - sort
 *  - prune
 *  - binary search
 *  - forall
 *  - find
 *  - largest
 *  - smallest
 */

#endif /* end of header guard */



