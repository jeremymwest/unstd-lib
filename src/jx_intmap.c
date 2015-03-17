/*******************************************************************************
 * 
 * Copyright (c) 2015, Jeremy West. Distributed under the MIT license.
 *
 ******************************************************************************/
#include "jx_private.h"

#define VALID(self) \
  assert(self && "Unexpected NULL reference.")

struct jx_intmap_vtable {
  void (*finalize)(jx_intmap *self);
  void* (*find)(const jx_intmap *self, int key);
  void* (*add)(jx_intmap *self, int key);
  void* (*put)(jx_intmap *self, int key, bool *out_isnew);
  bool (*remove)(jx_intmap *self, int key);
  void (*clear)(jx_intmap *self);
};

void jx_intmap_destroy(void *intmap) {
  jx_intmap *self = intmap;

  if (self && self->_vtable) {
    jx_intmap_clear(self);
    self->_vtable->finalize(self);
    memset(self, 0, sizeof *self);
  }
}

void* jx_intmap_find(const jx_intmap *self, int key) {
  VALID(self);
  return self->_vtable->find(self, key);
}

void* jx_intmap_add(jx_intmap *self, int key) {
  VALID(self);

  return self->_vtable->add(self, key);
}

void* jx_intmap_put(jx_intmap *self, int key, bool *out_isnew) {
  bool tmp;

  VALID(self);

  return self->_vtable->put(self, key, (out_isnew != NULL ? out_isnew : &tmp));
}

bool jx_intmap_remove(jx_intmap *self, int key) {
  VALID(self);

  return self->_vtable->remove(self, key);
}

void jx_intmap_clear(jx_intmap *self) {
  VALID(self);

  if (self->size > 0) {
    self->_vtable->clear(self);
    self->size = 0;
  }
}

/******************************************************************************/
/* hashtable */

void jx_intmap_init_hashtable(jx_intmap *out_self, size_t isz, 
    jx_destructor destroy);








