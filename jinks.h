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
  int size;
  size_t itemsize;
  jx_destructor _destroy;
  const struct jx_list_vtable *_vtable;
  union {
    struct jx_vector {
      size_t cap;
      unsigned char *array;
    } vector;
  } _impl;
} jx_list;

typedef struct {
  int size;
  size_t itemsize;
  jx_destructor _destroy;
  const struct jx_intmap_vtable *_vtable;
  union {
    struct jx_inthash {
      int table_size;
      struct jx_inthash_bucket {
        int key, size;
        size_t cap;
        void* items;
      } *buckets;
    } hashtable;
  } _impl;
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
/* smart pointers */

void jx_pointer_init(jx_pointer *out_self, size_t sz, jx_destructor destroy);

void jx_pointer_clone(const jx_pointer *self, jx_pointer *out_clone);

void jx_pointer_destroy(void *pointer);

/******************************************************************************/
/* array slices */

void jx_slice_init(jx_slice *out_self, size_t itemsize, int num);

void jx_slice_clone(const jx_slice *self, jx_slice *out_clone);

void jx_slice_destroy(void *slice);

void* jx_slice_at(const jx_slice *self, int i);

void jx_slice_reslice(const jx_slice *self, int start, int step, int count, 
    jx_slice *out_slice);

void jx_slice_copyto(const jx_slice *self, void *ref_array);

void* jx_slice_make_array(const jx_slice *self);

/******************************************************************************/
/* lists */

void jx_list_init_vector(jx_list *out_self, size_t isz, jx_destructor destroy);

void jx_list_destroy(void *list);

void* jx_list_at(const jx_list *self, int i);

void* jx_list_append(jx_list *self);

void* jx_list_prepend(jx_list *self);

void* jx_list_insert(jx_list *self, int i);

void jx_list_remove(jx_list *self, int i);

void jx_list_clear(jx_list *self);

/******************************************************************************/
/* integer maps */

void jx_intmap_init_hashtable(jx_intmap *out_self, size_t isz, 
    jx_destructor destroy);

void jx_intmap_destroy(void *intmap);

void* jx_intmap_find(const jx_intmap *self, int key);

void* jx_intmap_add(jx_intmap *self, int key);

void* jx_intmap_put(jx_intmap *self, int key, bool *out_isnew);

bool jx_intmap_remove(jx_intmap *self, int key);

void jx_intmap_clear(jx_intmap *self);

/******************************************************************************/
/* string maps */

void jx_stringmap_destroy(void *stringmap);

void* jx_stringmap_find(const jx_stringmap *self, const char *key); 

void* jx_stringmap_add(jx_stringmap *self, const char *key);

void* jx_stringmap_put(jx_stringmap *self, const char *key, bool *out_isnew);

bool jx_stringmap_remove(jx_stringmap *self, const char* key);

void jx_stringmap_clear(jx_stringmap *self);

/******************************************************************************/
/* queues */

void jx_queue_destroy(void *queue);

void* jx_queue_push(jx_queue *self);

bool jx_queue_pop(jx_queue *self);

#endif  /* header guard */



