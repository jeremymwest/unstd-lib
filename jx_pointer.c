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

jx_result jx_pointer_init(jx_pointer *out_self, size_t sz, 
    jx_destructor destroy) {
  JX_NOT_NULL(out_self);
  
  out_self->data = malloc(sizeof *out_self->data);
  if (NULL == out_self->data) return JX_OUT_OF_MEMORY;

  out_self->data->item = malloc(sz);
  if (NULL == out_self->data->item) { 
    free(out_self->data); out_self->data = NULL;
    return JX_OUT_OF_MEMORY;
  }

  out_self->data->destroy = destroy;
  out_self->data->refs = 1;
  return JX_OK;
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
    jx_destroy(self->data->destroy, self->data->item);
    /* free the block of memory */
    free(self->data->item);
    memset(self->data, 0, sizeof *self->data);
    free(self->data);
  }
  memset(self, 0, sizeof *self);
}

void* jx_pointer_get(const jx_pointer *self) {
  VALID(self);
  return self->data->item;
}

#ifdef JX_TESTING

static jx_pointer ptr_var, *ptr = &ptr_var;
static jx_pointer ptr_var2, *ptr2 = &ptr_var2;

jx_test pointer_clone_count() {
  double *vals;
  
  JX_CATCH(jx_pointer_init(ptr, 2*sizeof(double), NULL));
  vals = jx_pointer_get(ptr);
  vals[0] = 1.5;
  vals[1] = -2.5;

  /* clone the pointer */
  jx_pointer_clone(ptr, ptr2);

  JX_EXPECT(vals == jx_pointer_get(ptr2),
      "The new pointer is pointing to the wrong object.");

  /* now destroy the original pointer.  */
  jx_pointer_destroy(ptr);

  /* make sure the buffer is still there */
  JX_EXPECT(vals == jx_pointer_get(ptr2),
      "The data was deleted when the original pointer was destroyed.");

  JX_EXPECT(vals[0] == 1.5 && vals[1] == -2.5, 
      "The contents of the data changed.");

  jx_pointer_destroy(ptr2);
  vals = NULL;
  return JX_PASS;
}

static int destroy_calls;

static void destroy_int(void *item) {
  int *val = item;
  ++destroy_calls;
  *val = 0;
}

jx_test pointer_calls_destroy() {
  int *val;

  JX_EXPECT(JX_OK == jx_pointer_init(ptr, sizeof(int), destroy_int),
      "Error creating pointer: out of memory?");
  val = jx_pointer_get(ptr);
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


