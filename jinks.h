/******************************************************************************
 *
 * Copyright (c) 2015 Jeremy West. Distributed under the MIT license.
 *
 *****************************************************************************/
#ifndef JINKS_H
#define JINKS_H

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* If stdbool.h is included on your system */
#include <stdbool.h>
/* otherwise, you can use a custom definition */
/*
#define bool char
#define true 1
#define false 0
*/

/******************************************************************************/

typedef void (*jx_destructor)(void *item);
void jx_destroy(jx_destructor dstr, void *item);

#define JX_CLEAR(x) memset(x, 0, sizeof *x);

/******************************************************************************
 * Type definitions
 *
 * Ideally, users of this library would ignore these details. They are
 * provided here so that the types used in the library are available
 * everywhere and so that types can be included in user structs and defined on
 * the stack. But the members of these structs are subject to change WITHOUT
 * WARNING. Accessing members of these types is considered a violation of the
 * interface and may produce undefined behavior.
 *
 * In short, treat the types as opaque and use the library via the functions
 * defined in the various header files. Those define the public interface.
 *
 *****************************************************************************/

/* Structs and unions */

typedef struct {
  size_t size;
  unsigned char *data;
} jx_buffer;

typedef struct {
  struct pointer_data {
    bool free_item;
    int refs;
    void *item;
    jx_destructor dstr;
  } *data;
} jx_pointer;

typedef struct {
  int start, stride, count;
  jx_pointer ptr;
} jx_slice;

/******************************************************************************/

/* Validation Macros */

#define JX_NOT_NULL(x) assert(x && "Unexpected NULL.")
#define JX_RANGE(x, min, max) assert(x >= min && x < max \
    && "Argument out of bounds.")
#define JX_INTERVAL(x, sz, min, max) assert(x >= min && sz >= 0 && \
    x + sz <= max && "Interval out of bounds.")
#define JX_POSITIVE(x) assert(x > 0 && "Argument must be positive.")
#define JX_NOT_NEG(x) assert(x >= 0 && "Argument can't be negative.")
#define JX_ARRAY_SZ(sz, arr) \
  JX_NOT_NEG(sz); assert((sz == 0) == (arr == NULL) && \
    "Invalid object: array size is incorrect.")

/******************************************************************************/

/* Unit testing support */

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

#endif


#endif

