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

#ifdef JX_TESTING
#include <stdio.h>

/* the value that represents a pass */
const jx_test JX_PASS = { NULL, NULL, NULL, 0 };

static char teststr[128];
static char dots[75];

extern const struct unit_test all_tests[];
extern const int num_of_tests;

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
  int j, failures = 0;

  for (i = 0; i < sizeof(dots); ++i) {
    dots[i] = '.';
  }

  for (j = 0; j < num_of_tests; ++j) {
    if (!run_test(j, num_of_tests)) {
      ++failures;
    }
  }
  printf("\n");
  printf("\nResults: %d of %d tests passed.\n", num_of_tests-failures, 
      num_of_tests);

  return 0;
}

#endif



