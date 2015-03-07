/*******************************************************************************
 * 
 * Copyright (c) 2015, Jeremy West. Distributed under the MIT license.
 *
 ******************************************************************************/
#ifndef JX_SLICE_H
#define JX_SLICE_H
#include "jinks.h"

jx_result jx_slice_init(jx_slice *out_self, size_t itemsize, int count);

void jx_slice_destroy(void *slice);

int jx_slice_count(const jx_slice *self);

void* jx_slice_get(const jx_slice *self, int i);

void jx_slice_reslice(const jx_slice *self, int start, int step, int count,
    jx_slice *out_slice);

#endif /* end of header guard */

