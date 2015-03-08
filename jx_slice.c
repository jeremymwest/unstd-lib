/*******************************************************************************
 * 
 * Copyright (c) 2015, Jeremy West. Distributed under the MIT license.
 *
 ******************************************************************************/
#include "jx_slice.h"
#include "jx_pointer.h"

#define VALID(self) \
  JX_NOT_NULL(self); \
  JX_NOT_NEG(self->count)

jx_result jx_slice_init(jx_slice *out_self, size_t itemsize, int count) {
  JX_NOT_NULL(out_self);
  JX_POSITIVE(itemsize);
  JX_NOT_NEG(count);

  out_self->start = 0;
  out_self->stride = itemsize;
  out_self->count = count;
  JX_TRY(jx_pointer_init(&out_self->ptr, itemsize*count, NULL));
  VALID(out_self);

  return JX_OK;
}

void jx_slice_destroy(void *slice) {
  jx_slice *self = slice;
  VALID(self);

  jx_pointer_destroy(&self->ptr);
  memset(self, 0, sizeof *self);
}

int jx_slice_count(const jx_slice *self) {
  VALID(self);
  return self->count;
}

static int get_byte_pos(const jx_slice *self, int i) {
  return self->start + self->stride*i;
}

void* jx_slice_get(const jx_slice *self, int i) {
  unsigned char *arr;
  VALID(self);
  JX_RANGE(i, 0, self->count);
  arr = jx_pointer_get(&self->ptr);
  return &arr[get_byte_pos(self, i)];
}

void jx_slice_reslice(const jx_slice *self, int start, int step, int count,
    jx_slice *out_slice) {
  int last;

  VALID(self);
  JX_NOT_NEG(count);
  JX_NOT_NULL(out_slice);
  if (start < 0) start += self->count;
  JX_RANGE(start, 0, self->count);

  /* compute the possible size given the start and step sizes
   *    count: = min(count, available)
   */
  last = start + step*(count-1);
  if (last < 0) { /* step must be negative */
    /* largest possible count so that we don't go past 0 */
    count = -start / step + 1;
  } else if (last >= self->count) {
    count = (self->count - 1 - start + step) / step; 
  }
  last = start + step*(count-1);

  out_slice->count = count;
  out_slice->start = get_byte_pos(self, start);
  out_slice->stride = self->stride * step;
  jx_pointer_clone(&self->ptr, &out_slice->ptr);
  VALID(out_slice);
}

#ifdef JX_TESTING

static jx_slice slice_var, *slice = &slice_var;
static jx_slice slice_var2, *slice2 = &slice_var2;

jx_test slice_operations() {
  int i, j;

  jx_slice_init(slice, sizeof(int), 10);

  JX_EXPECT(jx_slice_count(slice) == 10,
      "Incorrect count for new slice.");

  for (i = 0; i < jx_slice_count(slice); ++i) {
    int *val = jx_slice_get(slice, i);
    JX_EXPECT(val != NULL, "Unexpected NULL value.");
    *val = i;
  }

  for (i = 0; i < jx_slice_count(slice); ++i) {
    int *val = jx_slice_get(slice, i);
    JX_EXPECT(val && *val == i, "Unexpected value at slice[i].");
  }

  /* should be: 8, 6, 4, 2 */
  jx_slice_reslice(slice, 8, -2, 4, slice2);
  JX_EXPECT(jx_slice_count(slice2) == 4,
      "Incorrect slice count for reslice.");

  for (i = 0; i < jx_slice_count(slice2); ++i) {
    int *val = jx_slice_get(slice2, i);
    JX_EXPECT(val && *val == 8-2*i,
        "The slice does not contain the correct values.");
  }

  /* kill the original slice, should not destroy the array. */
  jx_slice_destroy(slice);

  /* now reslice the new slice to reverse it: start at end (-1), step backward
   * (-1), and have the same count. */
  jx_slice_reslice(slice2, -1, -1, jx_slice_count(slice2), slice);
  JX_EXPECT(jx_slice_count(slice) == jx_slice_count(slice2),
      "Slices should have the same count.");

  for (i = 0, j = jx_slice_count(slice2)-1; 
      i < jx_slice_count(slice) && j >= 0; ++i, --j) {
    int *val1, *val2;
    val1 = jx_slice_get(slice, i);
    val2 = jx_slice_get(slice2, j);
    JX_EXPECT(val1 && val2 && *val1 == *val2 && val1 == val2,
        "The new slice didn't match up as expected.");
  }

  /* now the underlying array should die. */
  jx_slice_destroy(slice2);
  jx_slice_destroy(slice);
  return JX_PASS;
}

#endif


