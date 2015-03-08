/*******************************************************************************
 *
 * Copyright (c) 2015 Jeremy West. Distributed under the MIT license.
 *
 ******************************************************************************/
#include "jx_buffer.h"

#define VALID(self) \
  JX_NOT_NULL(self); \
  JX_ARRAY_SZ(self->size, self->data); \
  assert(is_valid_size(self->size) && \
      "Invalid object state: invalid size.")

static bool is_valid_size(int sz) {
  int test = 1;
  while (test && test != sz) {
    test <<= 1;
  }
  return test == sz;
}

jx_result jx_buffer_init(jx_buffer *out_self, int initial_size) {
  assert(out_self && "NULL pointer.");

  out_self->size = 0;
  out_self->data = NULL;
  JX_TRY(jx_buffer_resize(out_self, initial_size));
  return JX_OK;
}

jx_result jx_buffer_clone(const jx_buffer *self, jx_buffer *out_clone) {
  VALID(self);
  JX_NOT_NULL(out_clone);

  out_clone->data = NULL;
  JX_TRY(jx_alloc(&out_clone->data, self->size));
  out_clone->size = self->size;
  memcpy(out_clone->data, self->data, self->size);

  return JX_OK;
}

void jx_buffer_destroy(void *buffer) {
  jx_buffer *self = buffer;

  if (self) {
    JX_FREE_AND_NULL(self->data);
    self->size = 10; /* set to an invalid state for debugging purposes. */
  }
}

jx_result jx_buffer_reserve(jx_buffer *self, int size) {
  VALID(self);
  if (size > self->size) {
    return jx_buffer_resize(self, size);
  } else {
    return JX_OK;
  }
}


jx_result jx_buffer_resize(jx_buffer *self, int size) {
  int newsz;

  VALID(self);

  if (size > 0) {
    newsz = self->size ? self->size : 1;
    while (newsz > size) newsz >>= 1;
    while (newsz < size) newsz <<= 1;
  } else newsz = 0;

  JX_TRY(jx_alloc(&self->data, newsz));
  /* succeeded, so set size field */
  self->size = newsz;
  return JX_OK;
}

int jx_buffer_size(const jx_buffer *self) {
  VALID(self);
  return self->size;
}

void* jx_buffer_at(const jx_buffer *self, int i)
{
  unsigned char *arr;
  VALID(self);
  assert(i < self->size && "Invalid index.");

  arr = self->data;
  return &arr[i];
}

void* jx_buffer_data(const jx_buffer *self) {
  VALID(self);
  return self->data;
}

void jx_buffer_move_tail(jx_buffer *self, int pos, int delta) {
  void *movefrom, *moveto;
  int bytes;

  VALID(self);
  JX_RANGE(pos, 0, self->size);
  JX_RANGE(delta, -pos, self->size - pos + 1);

  bytes = self->size - pos;
  bytes -= (delta > 0 ? delta : 0);
  /* if the move is really large then there's nothing to move since
   * we are effectively sliding the entire block off the end of the buffer.
   */
  if (bytes > 0) {  
    movefrom = jx_buffer_at(self, pos);
    moveto = jx_buffer_at(self, pos + delta);
    memmove(moveto, movefrom, bytes);
  } 
}

/******************************************************************************/

#ifdef JX_TESTING

jx_buffer buf_var, *buf = &buf_var;
jx_buffer buf_var2, *buf2 = &buf_var2;

jx_test buffer_basic_init() {
  JX_CATCH(jx_buffer_init(buf, 0));
  JX_EXPECT(jx_buffer_size(buf) == 0, 
      "The capacity of the buffer should be 0.");
  jx_buffer_destroy(buf);
  return JX_PASS;
}

jx_test buffer_resize() {
  JX_CATCH(jx_buffer_init(buf, 3));
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

  JX_CATCH(jx_buffer_init(buf, 10*sizeof(int)));

  vals = jx_buffer_at(buf, 0);
  for (i = 0; i < 10; ++i) {
    vals[i] = i;
  }

  for (i = 0; i < 10; ++i) {
    int *val = jx_buffer_at(buf, i*sizeof(int));
    JX_EXPECT(*val == i, "Incorrect value.");
  }

  vals = jx_buffer_data(buf);
  JX_EXPECT(NULL != vals, "Null data returned.");
  for (i = 0; i < 10; ++i) {
    JX_EXPECT(vals[i] == i, "Incorrect value in buffer data.");
  }
  
  jx_buffer_destroy(buf);
  return JX_PASS;
}

jx_test buffer_reserve() {
  JX_CATCH(jx_buffer_init(buf, 4));
  JX_EXPECT(4 == jx_buffer_size(buf), "Incorrect buffer size.");
  JX_CATCH(jx_buffer_reserve(buf, 4));
  JX_EXPECT(4 == jx_buffer_size(buf), "Incorrect buffer size.");
  JX_CATCH(jx_buffer_reserve(buf, 2));
  JX_EXPECT(4 == jx_buffer_size(buf), "Buffer should not have shrunk.");
  JX_CATCH(jx_buffer_resize(buf, 10));
  JX_EXPECT(16 == jx_buffer_size(buf), "Buffer was not properly resized.");
  jx_buffer_destroy(buf);
  return JX_PASS;
}

jx_test buffer_clone() {
  jx_buffer buf2;
  double x = 22.2321;

  JX_CATCH(jx_buffer_init(buf, sizeof x));
  memcpy(jx_buffer_data(buf), &x, sizeof x);

  JX_CATCH(jx_buffer_clone(buf, &buf2));
  JX_EXPECT(sizeof x == jx_buffer_size(&buf2), "Cloned buffer has wrong size.");
  JX_EXPECT(0 == memcmp(jx_buffer_data(&buf2), &x, sizeof x),
      "Buffer has incorrect contents.");

  jx_buffer_destroy(buf);

  /* cloned buffer should still be active and have same data. */

  JX_EXPECT(sizeof x == jx_buffer_size(&buf2), "Cloned buffer has wrong size.");
  JX_EXPECT(0 == memcmp(jx_buffer_data(&buf2), &x, sizeof x),
      "Buffer has incorrect contents.");

  jx_buffer_destroy(&buf2);

  return JX_PASS;
}

#endif




