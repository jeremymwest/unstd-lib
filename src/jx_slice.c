/*******************************************************************************
 * 
 * Copyright (c) 2015, Jeremy West. Distributed under the MIT license.
 *
 ******************************************************************************/
#include "jx_private.h"

#define VALID(self) \
  JX_NOT_NULL(self); \
  JX_NOT_NEG(self->size); \
  JX_POSITIVE(self->itemsize); \
  JX_NOT_NULL(self->_ptr.item); \
  JX_NOT_NEG(self->_start); \
  assert(0 != self->_stride && "Stride should not be zero!")

void jx_slice_init(jx_slice *out_self, size_t itemsize, int num) {
  JX_NOT_NULL(out_self);
  JX_POSITIVE(itemsize);
  JX_NOT_NEG(num);

  out_self->itemsize = itemsize;
  out_self->size = num;
  out_self->_start = 0;
  out_self->_stride = 1;
  jx_pointer_init(&out_self->_ptr, num*itemsize, NULL);

  VALID(out_self);
}

void jx_slice_clone(const jx_slice *self, jx_slice *out_clone) {
  VALID(self);
  JX_NOT_NULL(out_clone);

  out_clone->size = self->size;
  out_clone->itemsize = self->itemsize;
  out_clone->_start = self->_start;
  out_clone->_stride = self->_stride;

  /* clone the smart pointer for the underyling array */
  jx_pointer_clone(&self->_ptr, &out_clone->_ptr);
}

void jx_slice_destroy(void *slice) {
  jx_slice *self = slice;

  VALID(self);

  jx_pointer_destroy(&self->_ptr);
  memset(self, 0, sizeof *self);
}

void* jx_slice_at(const jx_slice *self, int i) {
  unsigned char *arr;
  size_t pos;

  VALID(self);

  i = jxp_wrap_index(i, self->size);
  arr = self->_ptr.item;
  pos = self->itemsize*(self->_start + i*self->_stride);
  return &arr[pos];
}

void jx_slice_reslice(const jx_slice *self, int start, int step, int num, 
    jx_slice *out_slice) {
  int last;

  VALID(self);
  JX_NOT_NULL(out_slice);

  /* adjust the parameters so they are correct */
  start = jxp_wrap_index(start, self->size);
  last = start + step * ( num - 1);
  if (last < 0) {
    num = -start / step + 1;
  } else if (last >= self->size) {
    num = (self->size - 1 - start + step) / step;
  }
  step *= self->_stride;   /* step should be relative to this slice */

  jx_pointer_clone(&self->_ptr, &out_slice->_ptr);
  out_slice->itemsize = self->itemsize;
  out_slice->size = num;
  out_slice->_start = start;
  out_slice->_stride = step;

  VALID(out_slice);
}

void jx_slice_copyto(const jx_slice *self, void *ref_array) {
  int i;
  unsigned char *arr = ref_array;

  VALID(self);
  JX_NOT_NULL(ref_array);

  for (i = 0; i < self->size; ++i) {
    memcpy(&arr[i*self->itemsize], jx_slice_at(self, i), self->itemsize);
  }
}

void* jx_slice_make_array(const jx_slice *self) {
  void *arr;

  VALID(self);

  arr = jxp_alloc_exactly(NULL, self->size*self->itemsize);
  jx_slice_copyto(self, arr);
  return arr;
}

/******************************************************************************/

#ifdef JX_TESTING

jx_slice slice1, slice2;

jx_test slice_basics() {

  return JX_PASS;
}


#endif


