/******************************************************************************
 *
 * Copyright (c) 2015 Jeremy West. Distributed under the MIT license.
 *
 * This is the only header that must be included to use any of the
 * types or functions provided by the Jinks library. It must also be in the
 * include path of the library source files when they are compiled.
 * Documentation is minimal in this header. For detailed documentation, see
 * README.md
 *
 *****************************************************************************/
#ifndef JINKS_H
#define JINKS_H

#include <stdlib.h>
/* If stdbool.h is included on your system */
#include <stdbool.h>
/* otherwise, you can use a custom definition
#define bool char
#define true 1
#define false 0
*/

/******************************************************************************/
/* Handle out of memory */

typedef void (*jx_out_of_memory_handler)(bool *ref_recover);

void jx_register_memory_handler(jx_out_of_memory_handler handler);

/******************************************************************************/
/* Types defined by the system */

typedef void (*jx_destructor)(void *item);

typedef struct {
  void *item;
  struct {
    int refs;
    jx_destructor destroy;
  } *_info;
} jx_pointer;

typedef struct {
  size_t itemsize;
  int size, _start, _stride;
  jx_pointer _ptr;
} jx_slice;

typedef struct {
  void *item;
  const struct jx_spot_vtable *_vtable;
  union {
    int i;
  } _info;
} jx_spot;

typedef struct {
  int size;
  size_t itemsize;
  const struct jx_list_vtable *_vtable;
} jx_list;

typedef struct {
  int size;
  size_t itemsize;
  const struct jx_intmap_vtable *_vtable;
} jx_intmap;

typedef struct {
  int size;
  size_t itemsize;
  const struct jx_stringmap_vtable *_vtable;
} jx_stringmap;

typedef struct {
  int size;
  size_t itemsize;
  void *next;
  const struct jx_queue_vtable *_vtable;
} jx_queue;

/******************************************************************************/
/* Interfaces */

/* smart pointers */

void jx_pointer_init(jx_pointer *out_self, size_t sz, jx_destructor destroy);

void jx_pointer_clone(const jx_pointer *self, jx_pointer *out_clone);

void jx_pointer_destroy(void *pointer);

/* array slices */

void jx_slice_init(jx_slice *out_self, size_t itemsize, int num);

void jx_slice_clone(const jx_slice *self, jx_slice *out_clone);

void jx_slice_destroy(void *slice);

void* jx_slice_at(const jx_slice *self, int i);

void jx_slice_reslice(const jx_slice *self, int start, int step, int count, 
    jx_slice *out_slice);

void jx_slice_copyto(const jx_slice *self, void *ref_array);

void* jx_slice_make_array(const jx_slice *self);

/* spots */

bool jx_next(jx_spot *spot);

bool jx_prev(jx_spot *spot);

void jx_remove(jx_spot *spot);

int jx_indexof(const jx_spot *spot);

const char *jx_stringkey(const jx_spot *spot);

int *jx_intkey(const jx_spot *spot);

/* lists */

void jx_list_clone(const jx_list *self, jx_list *out_clone);

void jx_list_destroy(void *list);

void* jx_list_front(const jx_list *self, jx_spot *out_spot);

void* jx_list_back(const jx_list *self, jx_spot *out_spot);

void* jx_list_at(const jx_list *self, int i, jx_spot *out_spot);

void jx_list_prepend(jx_list *self, int num, jx_spot *out_spot);

void jx_list_append(jx_list *self, int num, jx_spot *out_spot);

void jx_list_insert(jx_list *self, int i, int num, jx_spot *out_spot);

void jx_list_remove(jx_list *self, int i, int num);

void jx_list_pop_back(jx_list *self, int num);

void jx_list_clear(jx_list *self);

/* integer maps */

void jx_intmap_clone(const jx_intmap *self, jx_intmap *out_clone);

void jx_intmap_destroy(void *intmap);

bool jx_intmap_find(const jx_intmap *self, int key, jx_spot *out_spot);

bool jx_intmap_add(jx_intmap *self, int key, jx_spot *out_spot);

bool jx_intmap_put(jx_intmap *self, int key, jx_spot *out_spot);

bool jx_intmap_remove(jx_intmap *self, int key);

void jx_intmap_clear(jx_intmap *self);

/* string maps */

void jx_stringmap_clone(const jx_stringmap *self, jx_stringmap *out_clone);

void jx_stringmap_destroy(void *stringmap);

bool jx_stringmap_find(const jx_stringmap *self, const char *key, 
    jx_spot *out_spot);

bool jx_stringmap_put(jx_stringmap *self, const char *key, jx_spot *out_spot);

bool jx_stringmap_remove(jx_stringmap *self, const char* key);

void jx_stringmap_clear(jx_stringmap *self);

/* queues */

void jx_queue_clone(const jx_queue *self, jx_queue *out_clone);

void jx_queue_destroy(void *queue);

void jx_queue_push(jx_queue *self);

void jx_queue_pop(jx_queue *self);


#endif  /* header guard */



