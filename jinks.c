/*******************************************************************************
 *
 * Copyright (c) 2015 Jeremy West. Distributed under the MIT license.
 *
 ******************************************************************************/
#include "jinks.h"

void jx_pass() {}

#ifdef JX_TESTING
struct unit_test {
  const char *name;
  bool (*func)(void);
};

// this automatically generated file includes all the test definitions
#include "test_list.in"

bool run_test(int i, int count) {
  const struct unit_test *test = &all_tests[i];
  printf("\nRunning %s (%d of %d)..........  ",
      test->name, i+1, count);
  if (test->func()) {
    printf("OK");
    return true;
  } else {
    printf("FAILED");
    return false;
  }
}

void main() {
  int i, count = (int) ( sizeof(all_tests) / sizeof(struct unit_test) );
  int failures = 0;
  for (i = 0; i < count; ++i) {
    if (!run_test(i, count)) {
      ++failures;
    }
  }
  printf("\n");
  printf("Results: %d of %d tests passed.", count-failures, count);
}

#endif

