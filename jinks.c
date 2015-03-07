/*******************************************************************************
 *
 * Copyright (c) 2015 Jeremy West. Distributed under the MIT license.
 *
 ******************************************************************************/
#include "jinks.h"

void jx_destroy(jx_destructor dstr, void *item) {
  /* NULL destructor is a no-op, NULL item should be ignored. */
  if (dstr && item) {
    dstr(item);
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
  JX_NOT_NULL(data);

  /* try to get a resized block (or a new one if the pointer is NULL */
  if (sz) {
    newdata = realloc(*data, sz);
    if (newdata) {   /* success, we're good */
      *data = newdata;
      return JX_OK;
    } else {
      /* leave the pointer alone, but indicate out of memory. */
      return JX_OUT_OF_MEMORY;
    }
  } else {
    JX_FREE_AND_NULL(*data);
    return JX_OK;   /* no memory requested: NULL array. */
  }
}

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



