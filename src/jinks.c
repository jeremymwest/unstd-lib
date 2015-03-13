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

void* jxp_alloc_exactly(void *ptr, size_t sz) {
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

void* jxp_alloc_atleast(void *ptr, size_t sz) {
  size_t cap;
  if (sz > 0) {  /* if a positive size, request a power of two */
    cap = 1;
    while (cap < sz) cap <<= 1;
  } else {
    cap = 0;
  }
  return jxp_alloc_exactly(ptr, cap);
}

int jxp_wrap_index(int i, int size) {
  JX_NOT_NEG(size);
  JX_RANGE(i, -size, size);

  return i >= 0 ? i : i + size;
}



