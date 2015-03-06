/*******************************************************************************
 *
 * Copyright (c) 2015 Jeremy West. Distributed under the MIT license.
 *
 ******************************************************************************/
#include "jx_buffer.h"

#define VALID(self) assert(self && "NULL pointer."); \
                    assert((self->data == NULL) == (self->size == 0) \
                        && "Invalid object state: size and array mismatch."); \
                    assert(is_valid_size(self->size) && \
                        "Invalid object state: invalid size.")

static bool is_valid_size(size_t sz) {
  size_t test = 1;
  while (test && test != sz) {
    test <<= 1;
  }
  return test == sz;
}

void jx_buffer_init(jx_buffer *out_self, size_t initial_size) {
  assert(out_self && "NULL pointer.");

  out_self->size = 0;
  out_self->data = NULL;
  if (initial_size > 0) {
    jx_buffer_resize(out_self, initial_size);
  }
}

void jx_buffer_destroy(void *buffer) {
  jx_buffer *self = buffer;

  if (self) {
    free(self->data);
    self->data = NULL;
    self->size = 10; /* set to an invalid state for debugging purposes. */
  }
}

void jx_buffer_resize(jx_buffer *self, size_t size) {

  VALID(self);

  if (size > 0) {
    if (!self->size) {
      self->size = 1;
    }
    while (self->size > size) {
      self->size >>= 1; // divide by two until smaller or equal
    }
    while (self->size < size) {
      self->size <<= 1; // multiply by two until at least as large
    }
    self->data = realloc(self->data, self->size);
  } else {
    self->size = 0;
    free(self->data);
    self->data = NULL;
  }
}

size_t jx_buffer_size(const jx_buffer *self) {
  VALID(self);

  return self->size;
}

void* jx_buffer_get(const jx_buffer *self, size_t index)
{
  VALID(self);
  assert(index < self->size && "Invalid index.");

  return &self->data[index];
}

#ifdef JX_TESTING

jx_buffer buf_var, *buf = &buf_var;

jx_test buffer_basic_init() {
  jx_buffer_init(buf, 0);

  JX_EXPECT(jx_buffer_size(buf) == 0, 
      "The capacity of the buffer should be 0.");

  jx_buffer_destroy(buf);
  return JX_PASS;
}

jx_test buffer_resize() {
  jx_buffer_init(buf, 3);

  JX_EXPECT(jx_buffer_size(buf) == 4,
      "The capacity of the buffer should be the next largest power of 2.");

  jx_buffer_resize(buf, 10);
  JX_EXPECT(jx_buffer_size(buf) == 16,
      "The capacity of the buffer should be the next largest power of 2.");

  jx_buffer_resize(buf, 6);
  JX_EXPECT(jx_buffer_size(buf) == 8,
      "The capacity of the buffer should be the next largest power of 2.");

  jx_buffer_destroy(buf);
  return JX_PASS;
}

jx_test buffer_get() {
  int i;
  int *vals;

  jx_buffer_init(buf, 10*sizeof(int));
  vals = jx_buffer_get(buf, 0);
  for (i = 0; i < 10; ++i) {
    vals[i] = i;
  }

  for (i = 0; i < 10; ++i) {
    int *val = jx_buffer_get(buf, i*sizeof(int));
    JX_EXPECT(*val == i, "Incorrect value.");
  }
  
  jx_buffer_destroy(buf);
  return JX_PASS;
}

#endif


