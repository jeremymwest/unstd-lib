/*******************************************************************************
 * 
 * Copyright (c) 2015, Jeremy West. Distributed under the MIT license.
 *
 ******************************************************************************/
#include "jx_private.h"
#define VALID(self) \
  assert(self && self->size >= 0 && self->itemsize > 0 && self->_vtable && \
      "List object is in an invalid state.")

struct jx_list_vtable {
  void (*finalize)(jx_list *self);
  void* (*at)(const jx_list *self, int i);
  void* (*append)(jx_list *self);
  void* (*prepend)(jx_list *self);
  void* (*insert)(jx_list *self, int i);
  void (*remove)(jx_list *self, int i);
  void (*clear)(jx_list *self);
};

void jx_list_destroy(void *list) {
  jx_list *self = list;
  if (self && self->_vtable) {
    jx_list_clear(self);
    self->_vtable->finalize(self);
    memset(self, 0, sizeof *self);
  }
}

void* jx_list_at(const jx_list *self, int i) {
  VALID(self);
  i = jxp_wrap_index(i, self->size);
  return self->_vtable->at(self, i);
}

void* jx_list_append(jx_list *self) {
  VALID(self);
  return self->_vtable->append(self);
}

void* jx_list_prepend(jx_list *self) {
  VALID(self);
  return self->_vtable->prepend(self);
}

void* jx_list_insert(jx_list *self, int i) {
  VALID(self);
  if (i == 0) {
    return jx_list_prepend(self);
  } else if (i == self->size) {
    return jx_list_append(self);
  } else {
    return self->_vtable->insert(self, jxp_wrap_index(i, self->size));
  }
}

void jx_list_remove(jx_list *self, int i) {
  VALID(self);
  i = jxp_wrap_index(i, self->size);
  self->_vtable->remove(self, i);
}

void jx_list_clear(jx_list *self) {
  if (self-> size > 0) {
    self->_vtable->clear(self);
  }
}

/******************************************************************************/
/* Vector Implementation */

static void jx_vector_finalize(jx_list *self) {
  struct jxp_vector *vector = &self->_impl.vector;
  free(vector->array);
}

static void* jx_vector_at(const jx_list *self, int i) {
  const struct jxp_vector *vector = &self->_impl.vector;
  return &vector->array[i*self->itemsize];
}

static void* jx_vector_append(jx_list *self) {    
  struct jxp_vector *vector = &self->_impl.vector;
  size_t req = ++self->size * self->itemsize;
  vector->array = jxp_reserve(vector->array, &vector->cap, req);
  return &vector->array[req - self->itemsize];
}

static void* jx_vector_prepend(jx_list *self) {
  struct jxp_vector *vector = &self->_impl.vector;
  size_t sz = self->size++ * self->itemsize;
  size_t req = sz + self->itemsize;
  vector->array = jxp_reserve(vector->array, &vector->cap, req);
  
  /* slide everything over */
  if (sz) {
    memmove(vector->array + self->itemsize, vector->array, sz);
  }
  /* return the first item */
  return vector->array;
}

static void* jx_vector_insert(jx_list *self, int i) {
  struct jxp_vector *vector = &self->_impl.vector;
  /* bytes to slide over for insert */
  size_t move = (self->size - i) * self->itemsize;
  size_t req = ++self->size * self->itemsize;
  unsigned char *loc;
  vector->array = jxp_reserve(vector->array, &vector->cap, req);
  loc = &vector->array[i * self->itemsize];
  memmove(loc + self->itemsize, loc, move);
  return loc;
}

static void jx_vector_remove(jx_list *self, int i) {
  struct jxp_vector *vector = &self->_impl.vector;
  /* item to delete */
  unsigned char *loc = &vector->array[i * self->itemsize];
  /* call destructor */
  if (self->_destroy) {
    self->_destroy(loc);
  }
  /* if we aren't deleting the last item, we have to shift items */
  if (i < --self->size) { 
    /* bytes to move */
    size_t move = (self->size - i) * self->itemsize;
    memmove(loc, loc + self->itemsize, move);
  }
}

static void jx_vector_clear(jx_list *self) {
  struct jxp_vector *vector = &self->_impl.vector;
  /* if there is a destructor, call it on each item */
  if (self->_destroy) {
    unsigned char *item = vector->array;
    int i;
    for (i = 0; i < self->size; ++i) {
      self->_destroy(item);
      item += self->itemsize;
    }
  }
  /* Nothing to do, just reset the size */
  self->size = 0;
}

static const struct jx_list_vtable jx_vector_vtable = {
  jx_vector_finalize,
  jx_vector_at,
  jx_vector_append,
  jx_vector_prepend,
  jx_vector_insert,
  jx_vector_remove,
  jx_vector_clear,
};

void jx_list_init_vector(jx_list *out_self, size_t isz, jx_destructor destroy) {
  struct jxp_vector *vector = &out_self->_impl.vector;
  JX_NOT_NULL(out_self);
  JX_POSITIVE(isz);

  out_self->size = 0;
  out_self->itemsize = isz;
  out_self->_destroy = destroy;
  out_self->_vtable = &jx_vector_vtable;

  vector->cap = 0;
  vector->array = NULL;
}

/******************************************************************************/
/* Deque Implementation */

static void jx_deque_finalize(jx_list *self) {

}

static void* jx_deque_at(const jx_list *self, int i) {
  return NULL;
}

static void* jx_deque_append(jx_list *self) {
  return NULL;
}

static void* jx_deque_prepend(jx_list *self) {
  return NULL;
}

static void* jx_deque_insert(jx_list *self, int i) {
  return NULL;
}

static void jx_deque_remove(jx_list *self, int i) {

}

static void jx_deque_clear(jx_list *self) {

}

static const struct jx_list_vtable jx_deque_vtable = {
  jx_deque_finalize,
  jx_deque_at,
  jx_deque_append,
  jx_deque_prepend,
  jx_deque_insert,
  jx_deque_remove,
  jx_deque_clear,
};

void jx_list_init_deque(jx_list *out_self, size_t isz, jx_destructor destroy) {
  JX_NOT_NULL(out_self);
  JX_POSITIVE(isz);

  out_self->size = 0;
  out_self->itemsize = isz;
  out_self->_destroy = destroy;
  out_self->_vtable = &jx_deque_vtable;
}



