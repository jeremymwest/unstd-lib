/*******************************************************************************
 *
 * Copyright (c) 2015 Jeremy West. Distributed under the MIT license.
 *
 ******************************************************************************/
#ifndef JX_BUFFER_H
#define JX_BUFFER_H
#include "jinks.h"

jx_result jx_buffer_init(jx_buffer *out_self, int initial_size);

jx_result jx_buffer_clone(const jx_buffer *self, jx_buffer *out_clone);

void jx_buffer_destroy(void *buffer);

/******************************************************************************/

int jx_buffer_size(const jx_buffer *self);

void* jx_buffer_at(const jx_buffer *self, int i);

void* jx_buffer_data(const jx_buffer *self);

/******************************************************************************/

jx_result jx_buffer_reserve(jx_buffer *self, int size);

jx_result jx_buffer_resize(jx_buffer *self, int size);

void jx_buffer_move_tail(jx_buffer *self, int pos, int delta);

#endif /* end of header guard */

