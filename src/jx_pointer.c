/*******************************************************************************
 * 
 * Copyright (c) 2015, Jeremy West. Distributed under the MIT license.
 *
 ******************************************************************************/
#include "jx_private.h"

#define VALID(self) \
  assert(self && "NULL argument."); \
  assert(self->_info && "Pointer object has already been destroyed."); \
  assert(self->_info->refs >= 0 && "Pointer object is in an invalid state.")

void jx_pointer_init(jx_pointer *out_self, size_t sz, jx_destructor destroy) {
  JX_NOT_NULL(out_self);

  out_self->_info = jx_alloc_exactly(NULL, sizeof *out_self->_info);
  out_self->item = jx_alloc_exactly(NULL, sz);
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
  if (self && self->_info) {
    if (--self->_info->refs <= 0) {
      if (self->_info->destroy && self->item) {
        self->_info->destroy(self->item);
      }
      free(self->item);
      free(self->_info);
    }
    memset(self, 0, sizeof *self);
  }
}



