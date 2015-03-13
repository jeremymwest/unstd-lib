/*******************************************************************************
 * 
 * Copyright (c) 2015, Jeremy West. Distributed under the MIT license.
 *
 ******************************************************************************/
#include "jx_private.h"

#define VALID(self) \
  JX_NOT_NULL(self); \
  JX_NOT_NULL(self->item); \
  JX_NOT_NEG(self->_info->refs)

void jx_pointer_init(jx_pointer *out_self, size_t sz, jx_destructor destroy) {
  JX_NOT_NULL(out_self);

  out_self->_info = jxp_alloc_exactly(NULL, sizeof *out_self->_info);
  out_self->item = jxp_alloc_exactly(NULL, sz);
  out_self->_info->destroy = destroy;
  out_self->_info->refs = 1;

  VALID(out_self);
}

void jx_pointer_clone(const jx_pointer *self, jx_pointer *out_clone) {
  VALID(self);
  JX_NOT_NULL(out_clone);

  /* copy the data straight across */
  *out_clone = *self;

  /* increment the count */
  ++self->_info->refs;

  VALID(out_clone);
}

void jx_pointer_destroy(void *pointer) {
  jx_pointer *self = pointer;

  VALID(self);

  if (--self->_info->refs <= 0) {
    if (self->_info->destroy) {
      self->_info->destroy(self->item);
    }
    free(self->item);
    free(self->_info);
  }
  memset(self, 0, sizeof *self);
}

/******************************************************************************/

#ifdef JX_TESTING

static jx_pointer ptr1, ptr2;

jx_test pointer_clone_count() {
  double *vals;
  
  jx_pointer_init(&ptr1, 2*sizeof(double), NULL);
  JX_EXPECT(NULL != ptr1.item, "Memory block for item not created.");
  vals = ptr1.item;
  vals[0] = 1.5;
  vals[1] = -2.5;

  /* clone the pointer */
  jx_pointer_clone(&ptr1, &ptr2);

  JX_EXPECT(vals == ptr2.item,
      "The new pointer is pointing to the wrong object.");

  /* now destroy the original pointer.  */
  jx_pointer_destroy(&ptr1);

  /* make sure the buffer is still there */
  JX_EXPECT(vals == ptr2.item,
      "The data was deleted when the original pointer was destroyed.");

  JX_EXPECT(vals[0] == 1.5 && vals[1] == -2.5, 
      "The contents of the data changed.");

  jx_pointer_destroy(&ptr2);
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

  jx_pointer_init(&ptr1, sizeof(int), destroy_int);
  JX_EXPECT(NULL != ptr1.item, "Unexpected NULL");
  val = ptr1.item;
  *val = 23;

  destroy_calls = 0;

  jx_pointer_clone(&ptr1, &ptr2);
  jx_pointer_destroy(&ptr1);
  JX_EXPECT(destroy_calls == 0, "Destroy was incorrectly called.");
  jx_pointer_clone(&ptr2, &ptr1);
  jx_pointer_destroy(&ptr2);
  JX_EXPECT(destroy_calls == 0, "Destroy was incorrectly called.");
  jx_pointer_destroy(&ptr1);
  JX_EXPECT(destroy_calls == 1, 
      "Destroy wasn't called when all pointers where destroyed.");

  return JX_PASS;
}

#endif


