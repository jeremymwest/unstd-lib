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
/* internal/private functions */

void* jxp_alloc_exactly(void *dataptr, size_t sz);

void* jxp_alloc_atleast(void *dataptr, size_t sz);

int jxp_wrap_index(int i, int size);

/******************************************************************************/
/* some assert macros for validating data */

#define JX_NOT_NULL(x) assert(x && "Unexpected NULL.")
#define JX_RANGE(x, min, max) assert(x >= min && x < max \
    && "Argument out of bounds.")
#define JX_INTERVAL(x, sz, min, max) assert(x >= min && sz >= 0 && \
    x + sz <= max && "Interval out of bounds.")
#define JX_POSITIVE(x) assert(x > 0 && "Argument must be positive.")
#define JX_NOT_NEG(x) assert(x >= 0 && "Argument can't be negative.")
#define JX_ARRAY_SZ(sz, arr) \
  JX_NOT_NEG(sz); assert(((sz == 0) || (arr != NULL)) && \
    "Invalid object: array size is incorrect.")

/* Unit testing framework */

#ifdef JX_TESTING

typedef struct {
  const char *file, *expr, *msg;
  int line;
} jx_test;

extern const jx_test JX_PASS;

#define JX_EXPECT(expr, msg) \
  if (!(expr)) { \
    jx_test result = { \
      __FILE__, #expr, msg, __LINE__ }; \
    return result; \
  }

#define JX_FAIL(msg) \
  JX_EXPECT(false, msg)

struct unit_test {
  const char *name;
  jx_test (*func)(void);
};

#endif  /* #ifdef JX_TESTING */
#endif /* end of header guard */



