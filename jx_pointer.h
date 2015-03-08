/*******************************************************************************
 * 
 * Copyright (c) 2015, Jeremy West. Distributed under the MIT license.
 *
 ******************************************************************************/
#ifndef JX_POINTER_H
#define JX_POINTER_H
#include "jinks.h"

jx_result jx_pointer_init(jx_pointer *out_self, size_t sz, 
    jx_destructor destroy);

void jx_pointer_clone(const jx_pointer *self, jx_pointer *out_clone);

void jx_pointer_destroy(void *pointer);

void* jx_pointer_get(const jx_pointer *self);

#endif /* end of header guard */

