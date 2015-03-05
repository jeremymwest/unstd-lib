/*******************************************************************************
 *
 * Copyright (c) 2015 Jeremy West. Distributed under the MIT license.
 *
 ******************************************************************************/
#include "jinks.h"

void jx_buffer_init(jx_buffer *out_self, size_t initial_size);

void jx_buffer_destroy(void *buffer);

void jx_buffer_resize(jx_buffer *self, size_t size);

size_t jx_buffer_capacity(const jx_buffer *self);



