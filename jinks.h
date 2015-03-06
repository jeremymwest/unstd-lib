/******************************************************************************
 *
 * Copyright (c) 2015 Jeremy West. Distributed under the MIT license.
 *
 *****************************************************************************/

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

typedef void (*jx_destructor)(void *arg);

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

// Typedefs
typedef struct jx_buffer jx_buffer;


// Structs and unions

struct jx_buffer {
  size_t size;
  unsigned char *data;
};


/******************************************************************************/

// Unit testing support

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


