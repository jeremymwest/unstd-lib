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
/* Error codes: The library is designed to minimize the number of functions
 * that may return an error and to minimize the amount of error checking done
 * in release builds. Anything that violates the contract of a function (such
 * as passing in invalid indices) is validated using assert statements that
 * are compiled out in a release build. 
 *
 * However, for some operations, runtime errors may be unavoidable. For
 * example, running out of memory, not being able to access a file, or finding
 * a file in an invalid format, for example. Essentially, anything that
 * depends on the state of the machine or the input of a user. In these cases,
 * asserts are inadequate. The following enum lists all the errors that may be
 * returned. Any function that may produce an error has a return value of
 * jx_result. If the value is zero (JX_OK) then the operation succeeded.
 * Otherwise, the value indicates the error that occurred. Hopefully, the
 * consistent use of error codes as return values should make it possible for
 * consuming code to catch and handle these errors in a meaningful way.
 *
 * The documentation for any function that returns a jx_result should indicate
 * which error codes are possible.
 *
 * In all cases, the jx_get_err_message function declared below will get some
 * kind of printable, human-readable error message indicating what has gone
 * wrong. But the message is not context-sensitive, it is just a constant
 * string that depends only on the error code.
 ******************************************************************************/

typedef enum {
  JX_OK = 0,
  JX_OUT_OF_MEMORY,
} jx_result;

const char* jx_get_error_message(jx_result result);

/******************************************************************************/

typedef void *jx_outptr;

typedef void (*jx_destructor)(void *item);

/*******************************************************************************
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
 ******************************************************************************/

typedef struct {
  struct pointer_data {
    bool free_item;
    int refs;
    void *item;
    jx_destructor destroy;
  } *data;
} jx_pointer;

typedef struct {
  int start, stride, count;
  jx_pointer ptr;
} jx_slice;

typedef struct {
  jx_destructor destroy;
  size_t isz, cap;
  int size;
  unsigned char *data;
} jx_vector;

/******************************************************************************/

/* Debug Validation Macros */
/* These all depend on assert.h and are only used if NDEBUG is not set, that
 * is, if it is not a release build.
 *
 * Including all these checks catches abuses of the API that otherwise
 * wouldn't show up and reduces the number of run-time errors that must be
 * reported.
 */

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

/******************************************************************************/

/* Other internal helper functions and macros that should be ignored by
 * consumers. */

/* call a function that returns a possible error code (jx_result). If it
 * fails, bubble the error code up the call-stack (by returning). Otherwise,
 * let this function continue.
 */
#define JX_TRY(stmt) \
{ \
  jx_result err = stmt; \
  if (err != JX_OK) { \
    return err; \
  } \
}

#define JX_SET(out_ptr, val) \
  if (NULL != out_ptr) { \
    *(void**)out_ptr = (val); \
  }

void jx_destroy(jx_destructor destroy, void *item);

void jx_destroy_range(jx_destructor destroy, int count, size_t sz, void *items);

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

#define JX_CATCH(stmt) \
  JX_EXPECT(JX_OK == stmt, "Unexpected error occurred.")

struct unit_test {
  const char *name;
  jx_test (*func)(void);
};

#endif  /* #ifdef JX_TESTING */
#endif  /* header guard */


