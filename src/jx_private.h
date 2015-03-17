/*******************************************************************************
 * 
 * Copyright (c) 2015, Jeremy West. Distributed under the MIT license.
 *
 ******************************************************************************/
#ifndef JX_PRIVATE_H
#define JX_PRIVATE_H

#include <string.h>
#include <assert.h>
#include "jinks.h"

/******************************************************************************/
/* macros for validating input arguments */

#define JX_NOT_NULL(arg) \
  assert(arg && "Argument cannot be NULL.")
#define JX_NOT_NEG(arg) \
  assert(arg >= 0 && "Argument cannot be negative.")
#define JX_POSITIVE(arg) \
  assert(arg > 0 && "Argument must be positive.")
#define JX_RANGE(arg, min, max) \
  assert(arg >= min && arg < max && "Argument is out of range.")

/******************************************************************************/
/* internal/private functions */

int jx_wrap_index(int i, int size);

void* jx_alloc_exactly(void *ptr, size_t sz);

void* jx_alloc_atleast(void *ptr, size_t sz);

void* jx_reserve(void *ptr, size_t *ref_cap, size_t required);

void* jx_reserve_items(void *ptr, size_t *ref_cap, size_t isz, int num);

struct jx_item_block_def {
  size_t block_size;
  int items_per_block;
};

void jx_choose_block_size(size_t isz, struct jx_item_block_def *out_blockdef);

void* jx_alloc_block(const struct jx_item_block_def *blockdef);

#endif /* end of header guard */



