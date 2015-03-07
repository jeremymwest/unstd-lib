/*******************************************************************************
 * 
 * Copyright (c) 2015, Jeremy West. Distributed under the MIT license.
 *
 ******************************************************************************/
#include "jx_pointer.h"
#define VALID(self) \
  JX_NOT_NULL(self); \
  JX_NOT_NULL(self->data); \
  JX_NOT_NEG(self->data->refs)

void* jx_pointer_init(jx_pointer *out_self, size_t sz, jx_destructor dstr) {
  JX_NOT_NULL(out_self);
  
  out_self->data = malloc(sizeof *out_self->data);
  if (sz > 0) {
    out_self->data->item = malloc(sz);
  } else {
    out_self->data->item = NULL;
  }
  out_self->data->dstr = dstr;
  out_self->data->refs = 1;
  return out_self->data->item;
}

void jx_pointer_clone(const jx_pointer *self, jx_pointer *out_clone) {
  VALID(self);

  /* point to the same data */
  out_clone->data = self->data;
  /* increment the reference counter */
  out_clone->data->refs++;

  VALID(out_clone);
}

void jx_pointer_destroy(void *pointer) {
  jx_pointer *self = pointer;

  VALID(self);
  /* no more references, clean pointer object. */
  if (--self->data->refs <= 0) {
    /* call the destructor */
    jx_destroy(self->data->dstr, self->data->item);
    /* free the block of memory */
    free(self->data->item);
    /* zero the data object, this way other pointers will know if the 
     * object got destroyed from underneath them (shouldn't happen). */
    JX_CLEAR(self->data);
    free(self->data);
  }
  JX_CLEAR(self); /* in either case, break connection to item */
}

void* jx_pointer_get(const jx_pointer *self) {
  VALID(self);
  return self->data->item;
}

#ifdef JX_TESTING

static jx_pointer ptr_var, *ptr = &ptr_var;
static jx_pointer ptr_var2, *ptr2 = &ptr_var2;
static jx_buffer *buf;

#include "jx_buffer.h"

jx_test pointer_clone_count() {
  double *vals;
  
  /* create a pointer object with enough space for a buffer */
  buf = jx_pointer_init(ptr, sizeof(jx_buffer), jx_buffer_destroy);

  /* initialize the buffer object */
  jx_buffer_init(buf, 2*sizeof(double));
  vals = jx_buffer_get(buf, 0);
  vals[0] = 1.5;
  vals[1] = -2.5;

  /* clone the pointer */
  jx_pointer_clone(ptr, ptr2);

  JX_EXPECT(buf == jx_pointer_get(ptr2),
      "The new pointer is pointing to the wrong object.");

  /* now destroy the original pointer.  */
  jx_pointer_destroy(ptr);

  /* make sure the buffer is still there */
  JX_EXPECT(buf == jx_pointer_get(ptr2),
      "The buffer was deleted when the original pointer was destroyed.");

  vals = jx_buffer_get(buf, 0);
  JX_EXPECT(vals[0] == 1.5 && vals[1] == -2.5, 
      "The contents of the buffer changed.");

  jx_pointer_destroy(ptr2);
  buf = NULL;
  return JX_PASS;
}

static int destroy_calls;

static void destroy_int(void *item) {
  int *val = item;
  ++destroy_calls;
  *val = 0;
}

jx_test pointer_calls_destroy() {
  int *val = jx_pointer_init(ptr, sizeof(int), destroy_int);
  JX_EXPECT(val != NULL, "Unexpected NULL");
  *val = 23;

  destroy_calls = 0;

  jx_pointer_clone(ptr, ptr2);
  jx_pointer_destroy(ptr);
  JX_EXPECT(destroy_calls == 0, "Destroy was incorrectly called.");
  jx_pointer_clone(ptr2, ptr);
  jx_pointer_destroy(ptr2);
  JX_EXPECT(destroy_calls == 0, "Destroy was incorrectly called.");
  jx_pointer_destroy(ptr);
  JX_EXPECT(destroy_calls == 1, 
      "Destroy wasn't called when all pointers where destroyed.");

  return JX_PASS;
}

#endif


