/*******************************************************************************
 *
 * Copyright (c) 2015 Jeremy West. Distributed under the MIT license.
 *
 ******************************************************************************/
#include "jinks.h"

void jx_destroy(jx_destructor destroy, void *item) {
  /* NULL destructor is a no-op, NULL item should be ignored. */
  if (destroy && item) {
    destroy(item);
  }
}

void jx_destroy_range(jx_destructor destroy, int count, size_t sz, 
    void *items) {
  if (destroy && items && count > 0 && sz > 0) {
    unsigned char *data = items;
    while (count-- > 0) {
      destroy(data);
      data += sz;
    }
  }
}

const char* jx_get_error_message(jx_result result) {
  switch (result) {
    case JX_OK: return "Operation succeeded.";
    case JX_OUT_OF_MEMORY: return "Insufficient memory. Cannot proceed.";
    default: return "An unknown error has occurred. This is bug.";
  }
}

jx_result jx_alloc(void **data, size_t sz) {
  void *newdata;
  JX_NOT_NULL(data); /* we must have been given a pointer to allocate to */

  /* try to get a resized block (or a new one if the pointer is NULL */
  newdata = realloc(*data, sz);
  if (sz && NULL == newdata) {   /* requested a non-zero block and failed */
    return JX_OUT_OF_MEMORY;
  } else {
    *data = newdata;
    return JX_OK;
  }
}

void jx_set_item(jx_outptr out_ptr, void *item) {
  if (out_ptr) { /* if NULL, they don't want the reference passed out */
    *(void**)out_ptr = item;
  }
}

/******************************************************************************/

#ifdef JX_TESTING

/* the value that represents a pass */
const jx_test JX_PASS = { NULL, NULL, NULL, 0 };

/* this automatically generated file includes all the test definitions */
#include "list_of_tests.h"

static char teststr[128];
static char dots[75];

bool run_test(int i, int count) {
  jx_test result;
  const struct unit_test *test = &all_tests[i];
  size_t len;
  
  sprintf(teststr, "Running %s (%d of %d) ", test->name, i+1, count);
  len = strlen(teststr);
  if (len < sizeof(dots)) { /* pad with dots  */
    printf("\n%s%s ", teststr, &dots[len]);
  } else {
    printf("\n%s ", teststr);
  }
  result = test->func();

  if (NULL == result.file) {
    printf("PASS");
    return true;
  } else {
    printf("FAIL");
    printf("\n\tAssertion failed in file (%s) on line (%d): %s.\n\t\t%s", 
        result.file, result.line, result.expr, result.msg);
    return false;
  }
}

int main() {
  size_t i;
  int j, count = (int) ( sizeof(all_tests) / sizeof(struct unit_test) );
  int failures = 0;

  for (i = 0; i < sizeof(dots); ++i) {
    dots[i] = '.';
  }

  for (j = 0; j < count; ++j) {
    if (!run_test(j, count)) {
      ++failures;
    }
  }
  printf("\n");
  printf("\nResults: %d of %d tests passed.\n", count-failures, count);

  return 0;
}

#endif



