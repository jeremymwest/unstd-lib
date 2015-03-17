/*******************************************************************************
 *
 * Copyright (c) 2015 Jeremy West. Distributed under the MIT license.
 *
 ******************************************************************************/
#include "jx_private.h"

static jx_out_of_memory_handler mem_handler = NULL;

void jx_register_memory_handler(jx_out_of_memory_handler handler) {
  mem_handler = handler;
}

void* jx_alloc_exactly(void *ptr, size_t sz) {
  ptr = realloc(ptr, sz);
  if (sz > 0 && NULL == ptr) {
    bool recover = false;
    if (NULL != mem_handler) {
      mem_handler(&recover);
    }
    if (!recover) {
      exit(1);
    }
  }
  return ptr;
}

void* jx_alloc_atleast(void *ptr, size_t sz) {
  size_t cap;
  if (sz > 0) {  /* if a positive size, request a power of two */
    cap = 1;
    while (cap < sz) cap <<= 1;
  } else {
    cap = 0;
  }
  return jx_alloc_exactly(ptr, cap);
}

int jx_wrap_index(int i, int size) {
  JX_NOT_NEG(size);
  JX_RANGE(i, -size, size);

  return i >= 0 ? i : i + size;
}

void* jx_reserve(void *ptr, size_t *ref_cap, size_t required) {
  JX_NOT_NULL(ref_cap);
  if (*ref_cap < required) {
    /* required must be at least one  */
    *ref_cap = 1;
    while (*ref_cap < required) *ref_cap <<= 1;
    ptr = jx_alloc_exactly(ptr, *ref_cap);
  }
  return ptr;
}

void* jx_reserve_items(void *ptr, size_t *ref_cap, size_t isz, int num) {
  return jx_reserve(ptr, ref_cap, num*isz);
}

static const size_t PREFERRED_BLOCK_SIZE = 1 << 14; /* 4kb block */

void jx_choose_block_size(size_t isz, struct jx_item_block_def *out_blockdef) {
  out_blockdef->block_size = PREFERRED_BLOCK_SIZE;
  while (out_blockdef->block_size < isz) {
    out_blockdef->block_size <<= 1;
  }
  out_blockdef->items_per_block = (int)(out_blockdef->block_size / isz); 
}

void* jx_alloc_block(const struct jx_item_block_def *blockdef) {
  return jx_alloc_exactly(NULL, blockdef->block_size);
}





