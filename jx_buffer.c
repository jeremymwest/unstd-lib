/*******************************************************************************
 *
 * Copyright (c) 2015 Jeremy West. Distributed under the MIT license.
 *
 ******************************************************************************/
#include "jx_buffer.h"

void jx_buffer_init(jx_buffer *out_self, size_t initial_size) {
  out_self->capacity = 0;
  out_self->data = NULL;
  if (initial_size > 0) {
    jx_buffer_resize(out_self, initial_size);
  }
}

void jx_buffer_destroy(void *buffer) {
  jx_buffer *self = buffer;
  free(self->data);
  self->data = NULL;
  self->capacity = 0;
}

void jx_buffer_resize(jx_buffer *self, size_t size) {
  if (size > 0) {
    while (self->capacity > size)
      self->capacity >>= 1; // divide by two until smaller or equal
    while (self->capacity < size)
      self->capacity <<= 1; // multiply by two until at least as large
    self->data = realloc(self->data, self->capacity);
  } else {
    self->capacity = 0;
    free(self->data);
    self->data = NULL;
  }
}

size_t jx_buffer_capacity(const jx_buffer *self) {
  return self->capacity;
}




